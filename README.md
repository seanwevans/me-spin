# 🌀 me@spin 🌀

A lightweight, single-header UTF-8 text spinner library for C.

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
#include <unistd.h> // For sleep()

int main() {
  // Create a spinner with default settings
  Spinner spinner = {
      .out = stdout,                 // Output to stdout
      .spinner_chars = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏", // Braille pattern
      .delay_ms = 100,               // Update every 100ms
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

    Compile with :

```bash gcc your_program.c -
    o your_program -
    pthread
```

    ##Built -
    in Spinner Styles 🎨

    me @spin comes with over 150 pre -
    defined spinner animations,
    from simple ASCII spinners to complex emoji sequences.Here are a few
            examples :

        | Name | Preview | | -- -- -- | -- -- -- -- - | | ARROWS | ←↖↑↗→↘↓↙ | |
        BRAILLE | ⠁⠂⠄⡀⢀⠠⠐⠈ | | CLOCK | 🕐🕑🕒🕓🕔🕕🕖🕗🕘🕙🕚🕛 | | DOTS | ⣾⣽⣻⢿⡿⣟⣯⣷ | |
        MOON_PHASES | 🌑🌒🌓🌔🌕🌖🌗🌘 |

        Access them using the `spinners` array :

```c Spinner spinner = {/* ... */};
spinner.spinner_chars = spinners[42].glyphs; // Use the 43rd spinner
```

    Or reference them by name :

```c spinner.spinner_chars = find_spinner_by_name("CLOCK");
```

    ##Usage 🔧

    ## #Multiple Spinners

        You can run multiple spinners simultaneously :

```c
#include "spinner.h"
#include <unistd.h>

    int
    main() {
  Spinner spinner1 = {.out = stdout,
                      .spinner_chars = spinners[0].glyphs,
                      .delay_ms = 100,
                      .fg_color = SPINNER_COLOR_GREEN,
                      .indent = 0};

  Spinner spinner2 = {
      .out = stdout,
      .spinner_chars = spinners[10].glyphs,
      .delay_ms = 200,
      .fg_color = SPINNER_COLOR_BLUE,
      .indent = 2 // Indented 2 spaces
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

    ## #Custom Spinners

        Create your own spinners with any UTF -
    8 characters :

```c
    // Custom spinner with emoji
    const char *custom_spinner = "😀😃😄😁😆😅🤣😂";
spinner.spinner_chars = custom_spinner;
```

    ##Example 🌟

    The repository includes an example.c file that demonstrates the library
    : To compile and run,
    simply execute :

```bash gcc -
    o spinner example.c./ spinner
```

        This will cycle through all built -
    in spinner styles.

    ##Compatibility 🧩

    -
    Works on any POSIX - compliant systems -
    Requires a terminal that supports UTF - 8 and
            ANSI color codes

            ##License 📄

            MIT License

            ##Contributing 🤝

            Contributions are welcome !Feel free to submit issues or
        pull requests.
