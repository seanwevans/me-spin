// spinner.h - single drop-in header utf8 text spinner library

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#define MAX_ACTIVE_SPINNERS 64

#ifndef SPINNER_H
#define SPINNER_H

#include <locale.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

// 🎨 Spinner Colors
typedef enum {
  SPINNER_COLOR_DEFAULT = -1,
  SPINNER_COLOR_BLACK = 0,
  SPINNER_COLOR_RED,
  SPINNER_COLOR_GREEN,
  SPINNER_COLOR_YELLOW,
  SPINNER_COLOR_BLUE,
  SPINNER_COLOR_MAGENTA,
  SPINNER_COLOR_CYAN,
  SPINNER_COLOR_WHITE
} SpinnerColor;

// ⚙️  Spinner
typedef struct {
  volatile int running;
  pthread_mutex_t lock;
  pthread_t tid;
  FILE *out;
  int indent;
  const char *spinner_chars;
  int delay_ms;
  SpinnerColor fg_color;
  SpinnerColor bg_color;
  char *message;
  int clobber_cursor;
  int show_cursor;
} Spinner;

static volatile sig_atomic_t spinner_term_width = 80;
static int spinner_stop(Spinner *spinner);
static volatile sig_atomic_t spinner_resize_requested = 0;
static Spinner *active_spinners[MAX_ACTIVE_SPINNERS];
static int active_spinner_count = 0;
static pthread_mutex_t spinner_registry_lock = PTHREAD_MUTEX_INITIALIZER;

// ⚙️⚙️ Spinners
typedef struct {
  const char *name;
  const char *glyphs;
} SpinnerParams;

// 🌀 Built-in Spinner Styles
static const SpinnerParams spinners[] = {
    {"ARROWS", "←↖↑↗→↘↓↙"},
    {"BADGES", "🔴🟠🟡🟢"},
    {"BARS", "▁▂▃▄▅"},
    {"BINARY", "01"},
    {"BLINK", "█ ▓ ▒ ░  ░ ▒ ▓ "},
    {"BLOCK_FILL", " ░▒▓█▓▒░"},
    {"BOMBS", "🧨💥🔥"},
    {"BONES", "🦴☠️💀🦷"},
    {"BOUNCEBALL", "⚫⚪"},
    {"BOUNCING", "⠁⠂⠄⠠⠐⠈⠉⠙⠚⠒⠂⠂⠒⠚⠙⠉⠈⠐⠠⠄⠂"},
    {"BOXES", "▖▘▝▗"},
    {"BRAILLE", "⠁⠂⠄⡀⢀⠠⠐⠈"},
    {"BRUSHES", "🖌️🖼️🎨🖍️"},
    {"BUBBLES", "∙°oO@Oo°∙"},
    {"BUTTERFLIES", "🦋🐛🦅🦄"},
    {"CACTUS", "🌵🏜️🦂🌞"},
    {"CAKES", "🍰🎂🧁🍮"},
    {"CALLS", "📞📱📲📳"},
    {"CARDS", "♠️♥️♦️♣️"},
    {"CHAOS", "@#%&$!?"},
    {"CHESS", "♙♘♗♖♕♔"},
    {"CHOCOLATES", "🍫🍩🍪🍬"},
    {"CHRISTMAS", "🎄🎅️❄️☃️"},
    {"CIRCLES", "◐◓◑◒"},
    {"CIRCLES_2", "◐◓◑◒"},
    {"CIRCUITRY", "⚡🔌💻📱"},
    {"CIRCUITS", "⚡🔌💾💿"},
    {"CLOCK", "🕐🕑🕒🕓🕔🕕🕖🕗🕘🕙🕚🕛"},
    {"CLOUDS", "☁️🌤️⛅🌥️☁️"},
    {"CLOUD_RAIN", "☁️🌧️💧☔"},
    {"CLOWNS", "🤡🎪🎉🎊"},
    {"COFFEE", "☕🫖🍵🧋"},
    {"COINS", "💰🪙💎🏆"},
    {"COLORS", "🟥🟦🟨🟩"},
    {"COMPUTER", "▌█▐░▒▓█▒░"},
    {"COMPUTERS", "🖥️💻📱📲"},
    {"CONSTRUCTION", "🏗️🧱🚧👷"},
    {"CORNERS", "⌜⌝⌟⌞"},
    {"CRYSTAL", "💎✨💠🔷"},
    {"CRYSTALS", "✧✦✧✦✧"},
    {"CRYSTALS_2", "💎🔮⚜️❄️"},
    {"CRYSTAL_BALL", "🔮✨⭐🌟"},
    {"CYCLE", "➀➁➂➃➄➅➆➇➈➉"},
    {"DIAMOND", "◇◆"},
    {"DICE", "⚀⚁⚂⚃⚄⚅"},
    {"DNA", "ᘛᘞᘟᘜ"},
    {"DOTS", "⣾⣽⣻⢿⡿⣟⣯⣷"},
    {"DOTS_2", "⚫⚪⬤⬭"},
    {"DRIP", "💧💦🌊💦"},
    {"ELEMENTS", "🔥💧🌪️🌱🪨"},
    {"EMERGENCY", "🚨🔺🚧⚠️"},
    {"EMOJI", "🤔📱💭✨"},
    {"EYES", "👀👁️👁️‍🗨️👁️"},
    {"FACES", "😐😑😶🙄😶‍🌫️"},
    {"FANTASY", "🧙‍♂️🧝‍♀️🧚🦄"},
    {"FAST_FOOD", "🍔🍟🍕🌭🌮"},
    {"FINGERPRINT", "⣾⣷⣯⣟⡿⢿⣻⣽"},
    {"FIREWORKS", "✨🎇️🎆🌟"},
    {"FLAMES", "🔥🟠🟡🔆✨💥"},
    {"FLIPS", "_-‾-"},
    {"FLOWERS", "🌸💮🏵️🌹🌺"},
    {"FLOWERS_2", "🌷🌸🌺🌼"},
    {"FOOD", "🍎🍊🍋🍏🍒🍇"},
    {"FRACTIONS", "½⅓¼⅕⅙⅐⅛"},
    {"FRAMES", "□■"},
    {"FRUITS", "🍓🍑🍍🥝🥭"},
    {"GALAXIES", "🌌🌠✨💫"},
    {"GAMES", "🎮🕹️🎲🎯"},
    {"GAMING_CONTROLLERS", "🎮🕹️🔴⚪"},
    {"GEARS", "⚙️⚙️⚙️⚙️"},
    {"GEMS", "💎♦️💍🎀"},
    {"GEOMETRY", "🔺🔻🔶🔷"},
    {"GLOW", "⊙⊗⊘⊚⊛"},
    {"GODOT", "◐◓◑◒"},
    {"GREEK_LETTERS", "αβγδε"},
    {"GROWING_PLANT", "🌱🌿🌳🌲"},
    {"HACKER", "👾💻🔓👨‍💻"},
    {"HALLOWEEN", "🎃👻💀🕷️"},
    {"HAND_GESTURES", "👍👌👎✋"},
    {"HEARTS", "💗💓💕💖💘"},
    {"HEART_BEAT", "♡♥︎"},
    {"HOLLOW", "⬒⬓⬔⬕"},
    {"HOURGLASS", "⌛⏳⌛⏳"},
    {"ICE_CREAM", "🍦🍧🍨🍡"},
    {"INSECTS", "🐞🐜🦗🦟"},
    {"INSTRUMENTS", "🎸🎺🎻🥁"},
    {"INSTRUMENTS_2", "🎺🎸🎻🥁"},
    {"KANJI", "日月火水木金土"},
    {"KATAKANA", "ｱｲｳｴｵ"},
    {"KEYBOARD", "⌨️💻🖱️🖲️"},
    {"LEAVES", "🍃🍂🥀🌿"},
    {"LIGHTS", "💡🔦🪔✨"},
    {"LINES", "|/-\\"},
    {"LIVES", "❤️🧡💛💙"},
    {"LOADING", "▰▱▰▱▱ ▰▰▱▱▱ ▰▰▰▱▱ ▰▰▰▰▱ ▰▰▰▰▰"},
    {"LOADING_BAR", "▱▰▱▰▱"},
    {"MAGIC", "⚡✨🔮⭐"},
    {"MAGIC_SWIRLS", "↺↻↼↽"},
    {"MATH", "∑∏∐∫∬∭⨌"},
    {"MEDICAL", "⚕️🩺🩹💉🩸"},
    {"MESSAGES", "✉️📨📱💬"},
    {"MICRO", "`^'´"},
    {"MONEY", "💲💰🪙💸"},
    {"MONSTERS", "👹👺🧟🧛"},
    {"MOON_PHASES", "🌑🌒🌓🌔🌕🌖🌗🌘"},
    {"MUSIC", "♩♪♫♬🎵🎶"},
    {"MUSICAL_INSTRUMENTS", "🎸🎹🎺🎻🥁"},
    {"MUSIC_2", "🎵🎶🎤🎧"},
    {"MUSIC_NOTES", "♫♪♬♩♮"},
    {"NARROW", "⠁⠃⠇⠧⠷⠿⠿⠷⠧⠇⠃"},
    {"NEBULA", "☁️🌫️🌁🌌"},
    {"NEON", "▓▒░▒▓"},
    {"NETWORK", "🌐📡🛰️🔗"},
    {"NETWORK_2", "🌐📡🛰️🔗"},
    {"NINJA", "👤🥷⚔️🔪"},
    {"NOTES", "♭♮♯"},
    {"NOTES_2", "♩♪♫♬"},
    {"NUMBERS", "1234567890"},
    {"ORBITS", "○◌◍◎◉"},
    {"PAINTS", "🎨🖼️🖌️🖍️"},
    {"PALETTE", "🎭🎨🖼️🖺"},
    {"PARTY", "🎉🎊🎈🥳"},
    {"PATTERNS", "▭▱△▽"},
    {"PENCILS", "✏️✐✑✒"},
    {"PEOPLE", "👩👨👧👦"},
    {"PHONES", "📞📱📲📳"},
    {"PHYSICS", "⚛️🧲🔋🔌"},
    {"PIRATES", "🏴‍☠️🦜⚓🏝️"},
    {"PLANETS", "☿♀♁♂♃♄♅♆♇"},
    {"PORTALS", "🌀🌌🕳️🌠"},
    {"POWERUPS", "⚡🔋💫✨"},
    {"PROCESSING", "▁▂▃▄▅"},
    {"PROGRESS", "◐◓◑◒"},
    {"PULSE", "█▓▒░ ░▒▓█"},
    {"PUNKS", "☠️💀☣️"},
    {"RAINBOWS", "🌈🎨💖✨"},
    {"RAINDROPS", "🌧️💧💦☔"},
    {"REACTIONS", "👍💯🔥⭐"},
    {"RECORDS", "📀🎵🎶🎧"},
    {"ROBOTS", "🤖📱💻🎮"},
    {"ROTATING_CROSS", "+x"},
    {"ROTATION", "↻↺"},
    {"RUNES", "🔮⚯️⚭⚬"},
    {"SCIENCE", "🔬🧪🧬🧫"},
    {"SCI_FI", "🤖🚀💫👾"},
    {"SCROLL", "📜🧾📑📄"},
    {"SEA_LIFE", "🐠🐟🐡🐙🦀"},
    {"SERVERS", "🖥️💾💻🔌"},
    {"SERVERS_2", "🖼️💾💿💻"},
    {"SHAPES", "◉○◎●"},
    {"SHAPES_2", "◉○◎●"},
    {"SMILEYS", "😊😄😃🙂"},
    {"SNAKE", "ˁ˚ᴥ˚ˀ ˁ˙ᴥ˙ˀ ˁ°ᴥ°ˀ"},
    {"SPARKLES", "✧✦✩✪"},
    {"SPRING", "🌸🌺🌷🌹"},
    {"SQUARES", "◻◼◽◾"},
    {"STARS", "⭐☆★⋆✦✨🌟💫"},
    {"STATIONERY", "📒📔📕📗"},
    {"STEAMPUNK", "⚙️🔩⚗️⌛"},
    {"SUMMER", "☀️🌞🌴🏖️"},
    {"SWEETS", "🍬🍭🍫🍩🍪"},
    {"TEA", "🧋🍵🥤🍹"},
    {"TERMINAL", "▮▯▭▬▫"},
    {"TIMER", "⏳⏱️⏲️⏰"},
    {"TOOLS", "🔨⛏️🔩🔧⚙️🛠️💡📐"},
    {"TREASURE", "🏵️🗝️💎📜"},
    {"TRIANGLES", "◢◣◤◥"},
    {"TRIANGLE_FLIP", "▲▼"},
    {"TROPHIES", "🏆🥇🏅🎖️"},
    {"TROPICAL", "🍍🥥🌴🌺"},
    {"TWINKLE", "✦✧✩✪✫✬✭✮✯✰"},
    {"VEHICLES", "🚗🚕🚙🚌🚓"},
    {"VIDEO", "📹📺📻📡"},
    {"WALK", "🚶‍♂️🏃‍♂️🕺💃"},
    {"WAVE", "~≃≈≋"},
    {"WEATHER", "☀️🌤️☁️🌧️⛈️❄️"},
    {"WINTER", "❄️☃️🎄🌨️"},
    {"WIZARDRY", "🧙‍♂️🔮✨🪄"},
};

// 🔡 UTF-8 Helpers
static const char *utf8_nth(const char *str, int n) {
  if (!str || n < 0)
    return NULL;
  const char *ptr = str;
  mbstate_t state = {0};
  wchar_t wc;
  size_t len;

  for (int i = 0; i <= n; ++i) {
    len = mbrtowc(&wc, ptr, MB_CUR_MAX, &state);
    if (len == (size_t)-1 || len == (size_t)-2 || len == 0)
      return NULL;
    if (i == n)
      return ptr;
    ptr += len;
  }
  return NULL;
}

static int utf8_len(const char *str) {
  if (!str)
    return 0;
  const char *ptr = str;
  mbstate_t state = {0};
  wchar_t wc;
  size_t len;
  int count = 0;

  while (*ptr) {
    len = mbrtowc(&wc, ptr, MB_CUR_MAX, &state);
    if (len == (size_t)-1 || len == (size_t)-2 || len == 0)
      break;
    count++;
    ptr += len;
  }
  return count;
}

static int utf8_display_width(const char *str) {
  if (!str)
    return 0;
  int width = 0;
  mbstate_t state = {0};
  wchar_t wc;
  size_t len;

  while (*str) {
    if ((unsigned char)*str < 0x80) {
      width += (*str != '\n' && *str != '\r') ? 1 : 0;
      str++;
      continue;
    }

    len = mbrtowc(&wc, str, MB_CUR_MAX, &state);
    if (len == (size_t)-1 || len == (size_t)-2 || len == 0)
      break;

    int w = wcwidth(wc);
    width += (w > 0) ? w : 0;
    str += len;
  }
  return width;
}

static int spinner_max_display_width(const char *spinner_chars) {
  int max = 0;
  int len = utf8_len(spinner_chars);
  for (int i = 0; i < len; i++) {
    const char *glyph = utf8_nth(spinner_chars, i);
    if (!glyph)
      continue;

    mbstate_t state = {0};
    wchar_t wc;
    size_t bytes = mbrtowc(&wc, glyph, MB_CUR_MAX, &state);
    if (bytes == (size_t)-1 || bytes == (size_t)-2 || bytes == 0)
      continue;

    int width = wcwidth(wc);
    if (width > max)
      max = width;
  }
  return (max > 0) ? max : 1;
}

// 🖥️ Terminal
static int get_terminal_width(FILE *stream) {
  if (!isatty(fileno(stream)))
    return 80;

  struct winsize w;
  if (ioctl(fileno(stream), TIOCGWINSZ, &w) != -1)
    spinner_term_width = w.ws_col;

  return spinner_term_width;
}

static void hide_cursor(FILE *out) {
  fprintf(out, "\033[?25l");
  fflush(out);
}

static void show_cursor(FILE *out) {
  fprintf(out, "\033[?25h");
  fflush(out);
}

// 🚨 Handlers
static void spinner_atexit_handler(void) { show_cursor(stdout); }

static void spinner_signal_handler(int signum) {
  show_cursor(stderr);
  signal(signum, SIG_DFL);
  raise(signum);
}

static void spinner_resize_handler(int signum) {
  (void)signum;
  spinner_resize_requested = 1;
}

// ✨ Spinner Constructor
static char *create_spinner(const char *first, ...) {
  if (!first)
    return NULL;

  va_list args;
  va_start(args, first);
  size_t total_len = strlen(first);
  const char *s = first;
  while ((s = va_arg(args, const char *)))
    total_len += strlen(s);
  va_end(args);

  char *result = (char *)malloc(total_len + 1);
  if (!result)
    return NULL;
  result[0] = '\0';

  va_start(args, first);
  strcat(result, first);
  while ((s = va_arg(args, const char *)))
    strcat(result, s);
  va_end(args);

  return result;
}

// 🧵 Spinner Thread
static void *spinner_thread(void *arg) {
  Spinner *s = (Spinner *)arg;
  int spinner_char_count = utf8_len(s->spinner_chars);
  int max_spinner_width = spinner_max_display_width(s->spinner_chars);
  size_t i = 0;
  static const char *fg_ansi[] = {"\033[30m", "\033[31m", "\033[32m",
                                  "\033[33m", "\033[34m", "\033[35m",
                                  "\033[36m", "\033[37m"};
  static const char *bg_ansi[] = {"\033[40m", "\033[41m", "\033[42m",
                                  "\033[43m", "\033[44m", "\033[45m",
                                  "\033[46m", "\033[47m"};
  const char *ansi_reset = "\033[0m";

  struct timespec ts = {.tv_sec = s->delay_ms / 1000,
                        .tv_nsec = (s->delay_ms % 1000) * 1000000};

  while (s->running) {
    nanosleep(&ts, NULL);

    if (spinner_resize_requested) {
      pthread_mutex_lock(&spinner_registry_lock);
      for (int k = 0; k < active_spinner_count; ++k) {
        get_terminal_width(active_spinners[k]->out);
      }
      spinner_resize_requested = 0;
      pthread_mutex_unlock(&spinner_registry_lock);
    }

    if (!s->clobber_cursor)
      fprintf(s->out, "\r\033[2K");

    pthread_mutex_lock(&s->lock);
    const char *msg = s->message ? s->message : "";
    pthread_mutex_unlock(&s->lock);


    for (int j = 0; j < s->indent; j++)
      fputc(' ', s->out);

    if (s->fg_color >= 0)
      fprintf(s->out, "%s", fg_ansi[s->fg_color]);

    if (s->bg_color >= 0)
      fprintf(s->out, "%s", bg_ansi[s->bg_color]);

    const char *glyph = utf8_nth(s->spinner_chars, i);
    int glyph_len = 0;
    int spinner_display_width = 1;

    if (glyph && *glyph) {
      wchar_t wc;
      mbstate_t state = {0};
      glyph_len = mbrtowc(&wc, glyph, MB_CUR_MAX, &state);

      if (glyph_len > 0) {
        spinner_display_width = wcwidth(wc);
        if (spinner_display_width < 1)
          spinner_display_width = 1;

        int left_pad = (max_spinner_width - spinner_display_width) / 2;
        int right_pad = max_spinner_width - spinner_display_width - left_pad;

        for (int p = 0; p < left_pad; ++p)
          fputc(' ', s->out);
        for (int b = 0; b < glyph_len; b++)
          fputc(glyph[b], s->out);
        for (int p = 0; p < right_pad; ++p)
          fputc(' ', s->out);
      } else {
        for (int p = 0; p < max_spinner_width - 1; ++p)
          fputc(' ', s->out);
        fputc('?', s->out);
      }
    } else {
      for (int p = 0; p < max_spinner_width - 1; ++p)
        fputc(' ', s->out);
      fputc('*', s->out);
    }

    fprintf(s->out, "%s", ansi_reset);

    if (msg)
      fprintf(s->out, " %s", msg);

    fprintf(s->out, "\033[K");
    fflush(s->out);

    i = (i + 1) % spinner_char_count;
  }

  fprintf(s->out, "\r\033[K");
  int width = get_terminal_width(s->out);
  for (int j = 0; j < width; j++)
    fputc(' ', s->out);
  fprintf(s->out, "\r\033[K");
  fflush(s->out);

  return NULL;
}

// ✏️  Post Spinner Message
static void spinner_post_message(Spinner *spinner, const char *new_msg) {
  if (!spinner || !new_msg)
    return;

  pthread_mutex_lock(&spinner->lock);
  free(spinner->message);
  spinner->message = new_msg ? strdup(new_msg) : NULL;
  pthread_mutex_unlock(&spinner->lock);
}

// ▶️  Start Spinner
static int spinner_start(Spinner *spinner) {
  if (spinner->running) {
    return 0;
  }
  // Initialize locale for character classification
  setlocale(LC_CTYPE, "");
  atexit(spinner_atexit_handler);

  pthread_mutex_init(&spinner->lock, NULL);
  pthread_mutex_lock(&spinner_registry_lock);
  if (active_spinner_count < MAX_ACTIVE_SPINNERS) {
    active_spinners[active_spinner_count++] = spinner;
  }
  pthread_mutex_unlock(&spinner_registry_lock);

  signal(SIGABRT, spinner_signal_handler);
  signal(SIGINT, spinner_signal_handler);
  signal(SIGSEGV, spinner_signal_handler);
  signal(SIGTERM, spinner_signal_handler);
  signal(SIGWINCH, spinner_resize_handler);

  if (!spinner->show_cursor)
    hide_cursor(spinner->out);

  spinner->running = 1;
  int rc = pthread_create(&spinner->tid, NULL, spinner_thread, spinner);
  if (rc != 0) {
    spinner->running = 0;
    return rc;
  }
  // pthread_detach(spinner->tid);

  spinner->message = spinner->message ? strdup(spinner->message) : NULL;
  return 0;
}

// ⏹️ Stop Spinner
static int spinner_stop(Spinner *spinner) {
  if (!spinner->running)
    return 0;

  spinner->running = 0;

  int rc = pthread_join(spinner->tid, NULL);

  pthread_mutex_destroy(&spinner->lock);
  pthread_mutex_lock(&spinner_registry_lock);
  for (int i = 0; i < active_spinner_count; ++i) {
    if (active_spinners[i] == spinner) {
      for (int j = i; j < active_spinner_count - 1; ++j) {
        active_spinners[j] = active_spinners[j + 1];
      }
      active_spinner_count--;
      break;
    }
  }
  pthread_mutex_unlock(&spinner_registry_lock);

  if (!spinner->show_cursor)
    show_cursor(spinner->out);

  free(spinner->message);
  spinner->message = NULL;
  return rc;
}

// 🔍 Find Spinner
#ifdef __cplusplus
extern "C" {
#endif
const char *find_spinner_by_name(const char *name) {
  int count = sizeof(spinners) / sizeof(spinners[0]);
  for (int i = 0; i < count; i++) {
    if (strcmp(spinners[i].name, name) == 0) {
      return spinners[i].glyphs;
    }
  }
  return NULL;
}
#ifdef __cplusplus
}
#endif
#endif // SPINNER_H
