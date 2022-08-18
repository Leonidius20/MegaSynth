module megasynth.filter;

double Filter::process(double inputValue) {
  buf0 += cutoff * (inputValue - buf0);
  buf1 += cutoff * (buf0 - buf1);
  switch (mode)
  {
  case Filter::LOW_PASS:
    return buf1;
  case Filter::HIGH_PASS:
    return inputValue - buf0;
  case Filter::BAND_PASS:
    return buf0 - buf1;
  default:
    return 0.0;
  }
}