#define PLUG_NAME "MegaSynth"
#define PLUG_MFR "Leonidius20"
#define PLUG_VERSION_HEX 0x00010000
#define PLUG_VERSION_STR "1.0.0"
#define PLUG_UNIQUE_ID '3c62'
#define PLUG_MFR_ID '971c'
#define PLUG_URL_STR "https://iplug2.github.io"
#define PLUG_EMAIL_STR "spam@me.com"
#define PLUG_COPYRIGHT_STR "Copyright 2022 Leonidius20"
#define PLUG_CLASS_NAME MegaSynth

#define BUNDLE_NAME "MegaSynth"
#define BUNDLE_MFR "Leonidius20"
#define BUNDLE_DOMAIN "com"

#define SHARED_RESOURCES_SUBPATH "MegaSynth"

// #define PLUG_CHANNEL_IO "1-1 2-2"
#if (defined(AAX_API) || defined(RTAS_API))
  #define PLUG_CHANNEL_IO "1-1 2-2"
#else
  // no audio input. mono or stereo output
  #define PLUG_CHANNEL_IO "0-1 0-2"
#endif

#define PLUG_LATENCY 0
#define PLUG_TYPE 1
#define PLUG_DOES_MIDI_IN 1
#define PLUG_DOES_MIDI_OUT 1
#define PLUG_DOES_MPE 0
#define PLUG_DOES_STATE_CHUNKS 0
#define PLUG_HAS_UI 1
#define PLUG_WIDTH 600
#define PLUG_HEIGHT 350
#define PLUG_FPS 60
#define PLUG_SHARED_RESOURCES 0
#define PLUG_HOST_RESIZE 0

#define AUV2_ENTRY MegaSynth_Entry
#define AUV2_ENTRY_STR "MegaSynth_Entry"
#define AUV2_FACTORY MegaSynth_Factory
#define AUV2_VIEW_CLASS MegaSynth_View
#define AUV2_VIEW_CLASS_STR "MegaSynth_View"

#define AAX_TYPE_IDS 'IPI1', 'IPI2'
#define AAX_TYPE_IDS_AUDIOSUITE 'IEA1', 'IEA2' // TODO: replace with appropriate for synth
#define AAX_PLUG_MFR_STR "Acme"
#define AAX_PLUG_NAME_STR "MegaSynth\nIPEF"
#define AAX_PLUG_CATEGORY_STR "Synth"
#define AAX_DOES_AUDIOSUITE 1

#define VST3_SUBCATEGORY "Instrument|Synth"

#define APP_NUM_CHANNELS 2
#define APP_N_VECTOR_WAIT 0
#define APP_MULT 1
#define APP_COPY_AUV3 0
#define APP_SIGNAL_VECTOR_SIZE 64

#define ROBOTO_FN "Roboto-Regular.ttf"

#define WAVEFORMS_FN "waveform.png"
#define FILTER_MODES_FN "filtermode.png"
