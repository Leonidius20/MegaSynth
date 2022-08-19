module megasynth.filter;

double Filter::process(double inputValue) {
  if (inputValue == 0.0)
    return 0.0;

  double calculatedCutoff = getCalculatedCutoff();

  // 1st order low pass
  buf0 += calculatedCutoff * (inputValue - buf0
    + feedbackAmount * (buf0 - buf1)); // resonance: adding bandpass output * by feedbackAmt
                                       // which is proportional to resonance

  // another 1st ordr lpass
  buf1 += calculatedCutoff * (buf0 - buf1);
  buf2 += calculatedCutoff * (buf1 - buf2);
  buf3 += calculatedCutoff * (buf2 - buf3);
                                       
  switch (mode)
  {
  case Filter::LOW_PASS:
    return buf3;              // low pass output (-12db, 2 filters)
  case Filter::HIGH_PASS:
    return inputValue - buf3; // buf0 is just the low frequencies, input - low frq. = high frq.
  case Filter::BAND_PASS:
    return buf0 - buf3;       // buf0 (gentle cut low pass) - buf1 (steeper cut low pass) = frequencies near cutoff
  default:
    return 0.0;
  }
}