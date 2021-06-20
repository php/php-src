--TEST--
Phi sources remove 001
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--EXTENSIONS--
opcache
--FILE--
<?php
function getOnlyMPEGaudioInfoBruteForce($info) {
    $Distribution['bitrate']      = array();
    $Distribution['frequency']    = array();
    $Distribution['layer']        = array();
    $Distribution['version']      = array();
    $Distribution['padding']      = array();

    $max_frames_scan = 5000;
    $frames_scanned  = 0;

    $previousvalidframe = $info['avdataoffset'];
    while ($info) {
        if (!isset($MPEGaudioHeaderDecodeCache[$head4])) {
            $MPEGaudioHeaderDecodeCache[$head4] = MPEGaudioHeaderDecode($head4);
        }
        if (!isset($MPEGaudioHeaderValidCache[$head4])) {
            $MPEGaudioHeaderValidCache[$head4] = MPEGaudioHeaderValid($MPEGaudioHeaderDecodeCache[$head4], false, false);
        }
        if ($MPEGaudioHeaderValidCache[$head4]) {

            if ($MPEGaudioHeaderLengthCache[$head4] > 4) {
                $WhereWeWere = mftell();
                $next4 = test(4);
                if ($next4[0] == "\xFF") {
                    if (!isset($MPEGaudioHeaderDecodeCache[$next4])) {
                        $MPEGaudioHeaderDecodeCache[$next4] = MPEGaudioHeaderDecode($next4);
                    }
                    if (!isset($MPEGaudioHeaderValidCache[$next4])) {
                        $MPEGaudioHeaderValidCache[$next4] = MPEGaudioHeaderValid($MPEGaudioHeaderDecodeCache[$next4], false, false);
                    }
                    if ($MPEGaudioHeaderValidCache[$next4]) {
                        getid3_lib::safe_inc($Distribution['bitrate'][$LongMPEGbitrateLookup[$head4]]);
                        getid3_lib::safe_inc($Distribution['layer'][$LongMPEGlayerLookup[$head4]]);
                        getid3_lib::safe_inc($Distribution['version'][$LongMPEGversionLookup[$head4]]);
                        getid3_lib::safe_inc($Distribution['padding'][intval($LongMPEGpaddingLookup[$head4])]);
                        getid3_lib::safe_inc($Distribution['frequency'][$LongMPEGfrequencyLookup[$head4]]);
                        if ($max_frames_scan && (++$frames_scanned >= $max_frames_scan)) {
                            foreach ($Distribution as $key1 => $value1) {
                                foreach ($value1 as $key2 => $value2) {
                                    $Distribution[$key1][$key2] = round($value2 / $pct_data_scanned);
                                }
                            }
                            break;
                        }
                        continue;
                    }
                }
                unset($next4);
            }

        }
    }
    return true;
}
?>
okey
--EXPECT--
okey
