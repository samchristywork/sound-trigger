#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SELECT_FORMAT(x)         \
  if (strcmp(optarg, #x) == 0) { \
    format = x;                  \
  }

/*
 * The usage message. This will print on error, or if the user selects the help
 * flag.
 */
void usage(char *argv[]) {
  fprintf(stderr,
          "Usage: %s [-t threshold (0-255)] [-o output file]\n"
          " -h\tDisplay this usage statement.\n"
          " -S\tExit when threshold hit.\n"
          "",
          argv[0]);
  exit(EXIT_FAILURE);
}

/*
 * A special assert to make the control flow below more natural.
 */
void alsa_assert(int expression, char *str1, const char *str2) {
  if (expression) {
    fprintf(stderr, "%s: %s\n", str1, str2);
    fflush(stderr);
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {

  FILE *outfile = 0;
  char *device = "default";
  unsigned int rate = 8000;
  int exit_on_threshold = 0;
  int threshold = -1;
  int num_samples = 16;
  snd_pcm_format_t format = SND_PCM_FORMAT_U8;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_t *capture_handle;

  /*
   * Handle program arguments.
   */
  int opt;
  char *optstring = "Sd:f:hn:o:r:t:";
  while ((opt = getopt(argc, argv, optstring)) != -1) {
    if (opt == 'S') {
      exit_on_threshold = 1;
    }
    if (opt == 'd') {
      device = strdup(optarg);
    }
    if (opt == 'f') {
      /*
       * NOTE: More formats may be available depending on your system. Check
       * out /usr/include/alsa.
       */
      format = SND_PCM_FORMAT_UNKNOWN;
      SELECT_FORMAT(SND_PCM_FORMAT_S8);
      SELECT_FORMAT(SND_PCM_FORMAT_U8);
      SELECT_FORMAT(SND_PCM_FORMAT_S16_LE);
      SELECT_FORMAT(SND_PCM_FORMAT_S16_BE);
      SELECT_FORMAT(SND_PCM_FORMAT_U16_LE);
      SELECT_FORMAT(SND_PCM_FORMAT_U16_BE);
      SELECT_FORMAT(SND_PCM_FORMAT_S24_LE);
      SELECT_FORMAT(SND_PCM_FORMAT_S24_BE);
      SELECT_FORMAT(SND_PCM_FORMAT_U24_LE);
      SELECT_FORMAT(SND_PCM_FORMAT_U24_BE);
      SELECT_FORMAT(SND_PCM_FORMAT_S32_LE);
      SELECT_FORMAT(SND_PCM_FORMAT_S32_BE);
      SELECT_FORMAT(SND_PCM_FORMAT_U32_LE);
      SELECT_FORMAT(SND_PCM_FORMAT_U32_BE);
    }
    if (opt == 'h') {
      usage(argv);
    }
    if (opt == 'n') {
      num_samples = atoi(optarg);
      if (!num_samples) {
        fprintf(stderr, "Number of samples must be a number other than zero.\n");
        usage(argv);
      }
    }
    if (opt == 'o') {
      outfile = fopen(optarg, "wb");
      if (outfile == 0) {
        perror("fopen");
        usage(argv);
      }
    }
    if (opt == 'r') {
      rate = atoi(optarg);
      if (!rate) {
        fprintf(stderr, "Value for rate must be a number other than zero.\n");
        usage(argv);
      }
    }
    if (opt == 't') {
      threshold = atoi(optarg);
    }
  }

  if (optind != argc) {
    fprintf(stderr, "Wrong number of arguments.\n");
    usage(argv);
  }

  /*
   * Initialize the sound device.
   */
  int err;
  err = snd_pcm_open(&capture_handle, device, SND_PCM_STREAM_CAPTURE, 0);
  alsa_assert(err < 0, "Cannot open audio device", snd_strerror(err));

  err = snd_pcm_hw_params_malloc(&hw_params);
  alsa_assert(err < 0, "Cannot allocated hw_params", snd_strerror(err));

  err = snd_pcm_hw_params_any(capture_handle, hw_params);
  alsa_assert(err < 0, "Cannot initialize hw_params", snd_strerror(err));

  err = snd_pcm_hw_params_set_access(capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
  alsa_assert(err < 0, "Cannot set access type", snd_strerror(err));

  err = snd_pcm_hw_params_set_format(capture_handle, hw_params, format);
  alsa_assert(err < 0, "Cannot set format", snd_strerror(err));

  err = snd_pcm_hw_params_set_rate_near(capture_handle, hw_params, &rate, 0);
  alsa_assert(err < 0, "Cannot set rate", snd_strerror(err));

  err = snd_pcm_hw_params_set_channels(capture_handle, hw_params, 1);
  alsa_assert(err < 0, "Cannot set channels", snd_strerror(err));

  err = snd_pcm_hw_params(capture_handle, hw_params);
  alsa_assert(err < 0, "Cannot set parameters", snd_strerror(err));

  snd_pcm_hw_params_free(hw_params);

  err = snd_pcm_prepare(capture_handle);
  alsa_assert(err < 0, "Cannot prepare PCM audio", snd_strerror(err));

  int buffer_frames = 128;
  char *buffer = malloc(128 * snd_pcm_format_width(format) / 8);
  bzero(buffer, 128 * snd_pcm_format_width(format) / 8);

  /*
   * Discard the first few samples.
   */
  for (int j = 0; j < 32; j++) {
    err = snd_pcm_readi(capture_handle, buffer, buffer_frames);
    alsa_assert(err != buffer_frames, "Cannot read audio interface",
                snd_strerror(err));
  }

  /*
   * Main loop
   */
  int running = 1;
  while (running) {
    float rms = 0;

    /*
     * Gather samples and populate an accumulator with the amplitude of each
     * sample.
     */
    for (int j = 0; j < num_samples; j++) {
      err = snd_pcm_readi(capture_handle, buffer, buffer_frames);
      alsa_assert(err != buffer_frames, "Cannot read audio interface",
                  snd_strerror(err));

      for (int i = 0; i < buffer_frames; i++) {
        float sample = buffer[i] + 128;
        rms += sample * sample;
      }

      /*
       * Write to a file if specified.
       */
      if (outfile) {
        fwrite(buffer, 1, buffer_frames, outfile);
      }
    }

    /*
     * Get the root mean square of the samples.
     */
    rms /= buffer_frames * num_samples;
    rms = sqrt(rms);

    /*
     * Do something if a threshold value is reached.
     */
    if (rms > threshold) {
      char buf[26];
      struct tm *tm_info;
      time_t timer = time(NULL);
      tm_info = localtime(&timer);
      strftime(buf, 26, "%Y-%m-%d %H:%M:%S", tm_info);

      printf("%f: %s\n", rms, buf);

      fflush(stdout);

      /*
       * Exit if the `-S` option was specified.
       */
      if (exit_on_threshold) {
        running = 0;
      }
    }
  }

  /*
   * Cleanup.
   */
  free(buffer);
  snd_pcm_close(capture_handle);
}
