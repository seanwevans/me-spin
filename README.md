# me@spin 🌀

A lightweight, modern, single-header UTF-8 text spinner library for C.

## Features ✨

- **Header-only library**: Just include `spinner.h` and you're good to go
- **UTF-8 support**: Display emojis and international characters beautifully
- **Colorful**: Customize your spinner with foreground and background colors
- **150+ built-in spinners**: From simple ASCII spinners to emoji animations
- **Thread-safe**: Run multiple spinners simultaneously
- **Terminal-aware**: Handles window resizing gracefully
- **Customizable**: Create your own spinner animations

## Installation 📦

Simply copy `spinner.h` to your project and include it:

```c
#include "spinner.h"
```

## Quick Start 🚀

```c
#include "spinner.h"
#include <unistd.h>  // For sleep()

int main() {
  // Create a spinner with default settings
  Spinner spinner = {
    .out = stdout,           // Output to stdout
    .spinner_chars = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏", // Braille pattern
    .delay_ms = 100,         // Update every 100ms
    .fg_color = SPINNER_COLOR_GREEN,
    .bg_color = SPINNER_COLOR_DEFAULT,
  };
  
  // Start the spinner
  spinner_start(&spinner);
  
  // Update the spinner message
  spinner_post_message(&spinner, "Loading...");
  
  // Do some work
  sleep(3);
  
  // Update message
  spinner_post_message(&spinner, "Still working...");
  sleep(2);
  
  // Stop the spinner
  spinner_stop(&spinner);
  
  printf("Done!\n");
  return 0;
}
```

Compile with:

```bash
gcc your_program.c -o your_program -pthread
```

## Built-in Spinner Styles 🎨

me@spin comes with over 150 pre-defined spinner animations, from simple ASCII spinners to complex emoji sequences. Here are a few examples:

| Name | Preview |
|------|---------|
| ARROWS | ←↖↑↗→↘↓↙ |
| BRAILLE | ⠁⠂⠄⡀⢀⠠⠐⠈ |
| CLOCK | 🕐🕑🕒🕓🕔🕕🕖🕗🕘🕙🕚🕛 |
| DOTS | ⣾⣽⣻⢿⡿⣟⣯⣷ |
| MOON_PHASES | 🌑🌒🌓🌔🌕🌖🌗🌘 |

Access them using the `spinners` array:

```c
Spinner spinner = { /* ... */ };
spinner.spinner_chars = spinners[42].glyphs;  // Use the 43rd spinner
```

Or reference them by name:

```c
// Find a spinner by name
const char* find_spinner_by_name(const char* name) {
  int count = sizeof(spinners) / sizeof(spinners[0]);
  for (int i = 0; i < count; i++) {
    if (strcmp(spinners[i].name, name) == 0) {
      return spinners[i].glyphs;
    }
  }
  return NULL;  // Not found
}

// Use it
spinner.spinner_chars = find_spinner_by_name("CLOCK");
```

## API Reference 📘

### Spinner Structure

```c
typedef struct {
  volatile int running;        // Internal use
  pthread_mutex_t lock;        // Internal use
  pthread_t tid;               // Internal use
  FILE *out;                   // Output file (usually stdout)
  int indent;                  // Number of spaces to indent
  const char *spinner_chars;   // Characters to use for spinning animation
  int delay_ms;                // Delay between frames in milliseconds
  SpinnerColor fg_color;       // Foreground color
  SpinnerColor bg_color;       // Background color
  char *message;               // Message to display next to spinner
  int clobber_cursor;          // Internal use
  int show_cursor;             // Whether to show the cursor
} Spinner;
```

### Colors

```c
typedef enum {
  SPINNER_COLOR_DEFAULT = -1,  // Terminal default
  SPINNER_COLOR_BLACK = 0,
  SPINNER_COLOR_RED,
  SPINNER_COLOR_GREEN,
  SPINNER_COLOR_YELLOW,
  SPINNER_COLOR_BLUE,
  SPINNER_COLOR_MAGENTA,
  SPINNER_COLOR_CYAN,
  SPINNER_COLOR_WHITE
} SpinnerColor;
```

### Functions

```c
// Start the spinner animation
static void spinner_start(Spinner *spinner);

// Stop the spinner animation
static void spinner_stop(Spinner *spinner);

// Update the spinner message
static void spinner_post_message(Spinner *spinner, const char *new_msg);
```

## Advanced Usage 🔧

### Multiple Spinners

You can run multiple spinners simultaneously:

```c
#include "spinner.h"
#include <unistd.h>

int main() {
  Spinner spinner1 = {
    .out = stdout,
    .spinner_chars = spinners[0].glyphs,
    .delay_ms = 100,
    .fg_color = SPINNER_COLOR_GREEN,
    .indent = 0
  };
  
  Spinner spinner2 = {
    .out = stdout,
    .spinner_chars = spinners[10].glyphs,
    .delay_ms = 200,
    .fg_color = SPINNER_COLOR_BLUE,
    .indent = 2  // Indented 2 spaces
  };
  
  spinner_start(&spinner1);
  spinner_post_message(&spinner1, "Task 1 processing...");
  
  sleep(1);
  
  spinner_start(&spinner2);
  spinner_post_message(&spinner2, "Task 2 processing...");
  
  sleep(3);
  
  spinner_stop(&spinner1);
  spinner_post_message(&spinner2, "Task 1 completed!");
  
  sleep(2);
  
  spinner_stop(&spinner2);
  
  return 0;
}
```

### Custom Spinners

Create your own spinners with any UTF-8 characters:

```c
// Custom spinner with emoji
const char *custom_spinner = "😀😃😄😁😆😅🤣😂";
spinner.spinner_chars = custom_spinner;
```

## Example 🌟

The repository includes an example.c file that demonstrates the library:

```c
// example.c
#include "spinner.h"

void simulate_proccess_for_(const unsigned sim_time_sec) {
  sleep(sim_time_sec);
}

int main() {
  int m = sizeof(spinners) / sizeof(spinners[0]);
  int i = 0;

  Spinner s = {.out = stdout,
               .fg_color = SPINNER_COLOR_WHITE,
               .bg_color = SPINNER_COLOR_RED,
               .delay_ms = 250};

  while (1) {
    s.spinner_chars = spinners[i].glyphs;
    spinner_start(&s);

    spinner_post_message(&s, "💥 Igniting the fire...");
    simulate_proccess_for_(1);

    spinner_post_message(&s, "🔥 Burning bright!");
    simulate_proccess_for_(4);

    spinner_post_message(&s, "🔥🔥 Woah!!!");
    simulate_proccess_for_(1);

    spinner_post_message(&s, "Extinguishing!");
    simulate_proccess_for_(1);

    spinner_post_message(&s, "Cooling down...");
    simulate_proccess_for_(3);

    spinner_stop(&s);
    i = (i + 1) % m;
  }

  return 0;
}
```

Compile and run it:

```bash
gcc example.c -o spinner -pthread
./spinner
```

This will cycle through all available spinner styles with a fire-themed animation.

## Compatibility 🧩

- Works on Linux, macOS, and other POSIX-compliant systems
- Requires a terminal that supports UTF-8 and ANSI color codes
- Requires pthread support

## Building 🔨

Compile your program with pthread support:

```bash
gcc your_program.c -o your_program -pthread
```

## License 📄

MIT License

## Contributing 🤝

Contributions are welcome! Feel free to submit issues or pull requests.

## Acknowledgments 🙏

- Inspired by various spinner libraries in the CLI ecosystem
- UTF-8 handling based on best practices for terminal applications
