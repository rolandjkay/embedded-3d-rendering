#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <argp.h>

const char *argp_program_version = "convert-bitmap v1.0";
const char *argp_program_bug_address = "<roland@enternijo.com>";
static char doc[] = "Convert text files containing two-colour bitmaps with "
                    "horizontal pixel layout to the vertacal layout used on "
                    "the SSD1306 display.\vNote that .c and .h suffixes will"
                    "be appended to OUTPUT_FILENAME";
static char args_doc[] = "INPUT_FILENAME OUTPUT_FILENAME";
static struct argp_option options[] = {
    { "symbol", 's', "SYMBOL", 0, "The symbol name to define in the output file."},
    { 0 }
};

typedef struct {
    //enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode;
    //bool isCaseInsensitive;
    const char* input_filename;
    const char* output_filename;
    const char* symbol_name;
} Arguments;

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    Arguments *arguments = state->input;
    switch (key) {
    case 's': arguments->symbol_name = arg; break;
    //case 'w': arguments->mode = WORD_MODE; break;
    //case 'i': arguments->isCaseInsensitive = true; break;
    case ARGP_KEY_ARG:
      switch (state->arg_num)
      {
        case 0:  arguments->input_filename = arg; break;
        case 1:  arguments->output_filename = arg; break;
        default: argp_usage(state);
      }
      break;

    case ARGP_KEY_END:
      if (!arguments->input_filename || !arguments->output_filename )
      {
        argp_usage (state);
      }

    default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };


/*
 * Read the given file into a char array.
 */
static char* read_file(const char* filename,
                       size_t* width,
                       size_t* height
                     )
{
  FILE* input_file = fopen(filename, "r");
  if (!input_file)
  {
    fprintf(stderr, "Failed to open file %s: %s\n", filename, strerror(errno));
    return NULL;
  }

  // Allocate buffer for output data.
  size_t data_size = 2048;
  char* data = malloc(data_size);

  // Allocate buffer for getline().
  char* line_buf = NULL;
  size_t line_buf_size = 0;

  size_t data_index = 0; // Index of next free byte
  size_t line_num = 0;   // Line num in file that we are reading.
  *width = 0;            // Don't know width yet
  *height = 0;

  while (getline(&line_buf, &line_buf_size, input_file) != -1)
  {
      ++line_num;
      size_t size_before_line = data_index;

      // Stop at end of line or comment delimiter
      for (char* p = line_buf; *p != '\0' && *p != '#'; ++p)
      {
        // Read any 1s or 0s into data; ignore anything else
        if (*p == '0' || *p == '1')
        {
          data[data_index++] = *p;

          // Allocate more memory, if needed.
          if (data_index == data_size)
          {
            data_size += 2048;
            data = realloc(data, data_size);
            if (!data)
            {
              fprintf(stderr, "Out of memory processing file %s\n", filename);
              goto error;
            }
          }
        }
      } // EOL

      // If this was the first line, then it determines 'width'
      if (data_index - size_before_line == 0)
      {
        /* do nothing */
      }
      // Ignore blank lines
      else if (*width == 0)
      {
        *width = data_index;
        ++*height;
      }
      // Else, line length had better match width
      else if (*width != data_index - size_before_line)
      {
        fprintf(stderr, "Incorrect number of pixels on line %zu\n", line_num);
        goto error;
      }
      else
      {
        ++*height;
      }
  }

  if (errno != 0)
  {
    fprintf(stderr, "File read error %s: %s\n", filename, strerror(errno));
    goto error;
  }

  return data;

error:
  fclose(input_file);
  free(line_buf);
  free(data);
  return NULL;
}

static char* heap_strcat(const char* s1, const char* s2)
{
  size_t new_str_len = strlen(s1) + strlen(s2) + 1;
  char* new_str = malloc(new_str_len);
  if (!new_str)
  {
    return NULL;
  }

  strlcpy(new_str, s1, new_str_len);
  strlcat(new_str, s2, new_str_len);

  return new_str;
}

/*
 * Transpose 8x8 blocks of each sprite to match the screen buffer format
 * of the SSD1306.
 */
int main(int argc, char *argv[])
{
  int prog_retval = 0;
  Arguments arguments = {.symbol_name = "bitmap_data"};
  size_t width, height, array_size = 0;

  /*
   * Parse command line args
   */
   argp_parse (&argp, argc, argv, 0, 0, &arguments);

   char* c_filename = heap_strcat(arguments.output_filename, ".c");
   char* h_filename = heap_strcat(arguments.output_filename, ".h");
   if (!c_filename || !h_filename) { goto error; }

  char* bin_data = read_file(arguments.input_filename, &width, &height);

  if (!bin_data)
  {
    goto error;
  }

  printf("Read %zux%zu bitmap from %s\n", width, height, arguments.input_filename);

  FILE* c_file = fopen(c_filename, "w");
  if (!c_file)
  {
    fprintf(stderr, "File open error %s: %s\n", arguments.output_filename, strerror(errno));
    goto error;
  }

  fprintf(c_file,
    "#ifdef __AVR\n"
    "#  include <avr/pgmspace.h>\n"
    "#else\n"
    "#  include \"../macos/pgmspace.h\"\n"
    "#endif\n\n"
    "const unsigned char %s[] PROGMEM = {\n", arguments.symbol_name);

  // A block is eight pixels high stretching the width of the bitmap.
  for (size_t block_start = 0; block_start < width*height; block_start += width * 8)
  {
    for (size_t column_index = 0; column_index < width; ++column_index)
    {
      uint8_t byte = 0;

      for (size_t row_index = 0; row_index < 8; ++row_index)
      {
        size_t offset = block_start + row_index * width + column_index;
        // If we've not run off the end of the buffer and the value is 1...
        byte |= (offset < width*height && bin_data[offset] == '1') ? (1<<row_index) : 0;

      }

      // EOL every 16 bytes
      if (column_index != 00 && column_index % 16 == 0)
      {
        fprintf(c_file, "\n");
      }

      fprintf(c_file, "0x%02x,", byte);
      ++array_size;
    }
    fprintf(c_file, "\n\n"); // End of block
  }
  fprintf(c_file, "};\n");

  FILE* header_file = fopen(h_filename, "w");
  if (!header_file)
  {
    fprintf(stderr, "File open error %s: %s\n", h_filename, strerror(errno));
    goto error;
  }
  fprintf(header_file, "#ifndef _BITMAP_%s_H\n", arguments.symbol_name);
  fprintf(header_file, "#define _BITMAP_%s_H\n", arguments.symbol_name);
  fprintf(header_file, "\n"
                       "#ifdef __AVR\n"
                       "#  include <avr/pgmspace.h>\n"
                       "#else\n"
                       "#  include \"../macos/pgmspace.h\"\n"
                       "#endif\n"
                       "\n"
                       "\nconst unsigned char %s[%zu] PROGMEM;\n\n",
                       arguments.symbol_name,
                       array_size);
  fprintf(header_file, "#endif\n");

exit:
  free(h_filename);
  free(c_filename);
  fclose(c_file);
  fclose(header_file);
  free(bin_data);
  return prog_retval;

error:
  prog_retval = 255;
  goto exit;
}
