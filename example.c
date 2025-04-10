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
               .delay_ms = 50};

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
