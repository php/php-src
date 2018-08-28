--TEST--
Bug #76738 Wrong handling of output buffer
--SKIPIF--
<?php
require_once('skipif.inc');
if (!extension_loaded('mbstring')) die('skip mbstring extension not available');
?>
--FILE--
<?php declare(strict_types=1);
$test_string = base64_decode('PGRpdiBjbGFzcz0idGlueW1jZS1nZW5lcmF0ZWQtcm9vdC1ibG9jayIgc3R5bGU9Im1hcmdpbjogMHB4OyBwYWRkaW5nOiAwcHg7Ij4KPGRpdiBzdHlsZT0iZmxvYXQ6IHJpZ2h0OyI+PGltZyBhbHQ9IkZvdG8gUmVpbmRsIEhhcmFsZCIgc3JjPSIvcnRlL3VwbG9hZC9mb3RvLmpwZyIgc3R5bGU9IndpZHRoOiAyNTBweDsgaGVpZ2h0OiAyNDlweDsiPjwvZGl2Pgo8ZGl2IHN0eWxlPSJ3aGl0ZS1zcGFjZTogbm93cmFwOyI+CjxwIHN0eWxlPSJtYXJnaW46IDBweDsiPlJlaW5kbCBIYXJhbGQmdXVtbDs8YnI+IFNpZWdmcmllZGdhc3NlIDIyLTI0LzIvNDxicj4gMTIxMCBXaWVuPGJyPiAoKzQzKSAwNjc2IDQwIDIyMSA0MDxicj4gW2JlZm9yZWxpbmtzXTxhIGhyZWY9Ii9zaG93X2NvbnRlbnQucGhwP3NpZD0xMDciPnN1cHBvcnRAcmhzb2Z0Lm5ldDwvYT48YnI+IFtiZWZvcmVsaW5rc108YSBocmVmPSIvcmVpbmRsLWhhcmFsZC52Y2YiPlYtQ2FyZCBoZXJ1bnRlcmxhZGVuPC9hPjxicj4gW2JlZm9yZWxpbmtzXTxhIHRhcmdldD0iX2JsYW5rIiBocmVmPSIvZ3BnL3N1cHBvcnRfcmhzb2Z0Lm5ldC5wdWIudHh0Ij5NYWlsIEdQRy1LZXk8L2E+PC9wPgo8L2Rpdj4KPGRpdiBzdHlsZT0iZmxvYXQ6IG5vbmU7IHBhZGRpbmctdG9wOiAxNXB4OyI+Cjxocj4KPGI+R2VidXJ0c2RhdHVtOiA8L2I+MTIuIE5vdmVtYmVyIDE5NzcsIE1pc3RlbGJhY2gsIE4mT3VtbDsKPGg0PkJFUlVGU0VSRkFIUlVORzwvaDQ+CjxoNT4xOTkzIC0gMjAwMCBJbmcuIEdpbmRsIEdtYkggMjEyMCBXb2xrZXJzZG9yZjwvaDU+Cjx1bD4KPGxpPk1vbnRhZ2UgdW5kIFdhcnR1bmcgdm9uICZvdW1sO2ZmZW50bGljaGVuIEJlbGV1Y2h0dW5nZW48L2xpPgo8bGk+V2FydHVuZyB1bmQgRXJyaWNodHVuZyB2b24gU2lnbmFsYW5sYWdlbjwvbGk+CjxsaT5Xb2huYmF1LUluc3RhbGxhdGlvbmVuPC9saT4KPGxpPkluZnJhc3RydWt0dXJhdXNiYXUgaW4gWnVzYW1tZW5hcmJlaXQgbWl0IGRlciBFVk4gTmllZGVyJm91bWw7c3RlcnJlaWNoPC9saT4KPC91bD4KPGRpdiBjbGFzcz0idGlueW1jZS1nZW5lcmF0ZWQtcm9vdC1ibG9jayIgc3R5bGU9Im1hcmdpbjogMHB4OyBwYWRkaW5nOiAwcHg7Ij4KPGg1PjIwMDEgLSAyMDAyIE4mT3VtbDsgVm9sa3NoaWxmZSAyMTMwIE1pc3RlbGJhY2g8L2g1Pgo8L2Rpdj4KPHVsPgo8bGk+RWxla3Ryb2luc3RhbGxhdGlvbiwgVGVjaG5pc2NoZXIgU3VwcG9ydDwvbGk+CjwvdWw+CjxoNT4yMDAxIC0gMjAwNyBFaW56ZWx1bnRlcm5laG1lcjwvaDU+Cjx1bD4KPGxpPkVudHdpY2tsdW5nIHZvbiBEYXRlbmJhbmstTCZvdW1sO3N1bmdlbiwgSW50ZXJuZXQvSW50cmFuZXQtQXBwbGlrYXRpb25lbjwvbGk+CjxsaT5OZXR6d2Vyay0gdW5kIFNlcnZlcnRlY2huaWs8L2xpPgo8bGk+Q29udGVudC1NYW5hZ21lbnQtU3lzdGVtZTwvbGk+CjxsaT5FLUJ1c2luZXNzPC9saT4KPC91bD4KPGg1PnNlaXQgMjAwODwvaDU+Cjx1bD4KPGxpPjxhIGhyZWY9Imh0dHA6Ly93d3cudGhlbG91bmdlLm5ldC8iIHRhcmdldD0icmh3aW4iIG9uY2xpY2s9InJod2luZm9jdXMoKTsiPnRoZWxvdW5nZSBpbnRlcmFjdGl2ZSBkZXNpZ248L2E+PC9saT4KPGxpPlNvZnR3YXJlLUVudHdpY2tsdW5nPC9saT4KPGxpPlRlY2huaXNjaGUgQWRtaW5pc3RyYXRpb248L2xpPgo8L3VsPgo8aHI+CjxoND5LVVJTRSBVTkQgU0NIVUxVTkdFTjwvaDQ+Cjx1bD4KPGxpPjEwLjA4LiAtIDA5LjEwLjIwMDIgTWljcm9zb2Z0IENlcnRpZmllZCBQcm9mZXNzaW9uYWw8L2xpPgo8bGk+MjkuMDYuIC0gMDkuMDguMjAwMiBXaW5kb3dzIDIwMDAgSW5zdGFsbGF0aW9uIHVuZCBWZXJ3YWx0dW5nPC9saT4KPGxpPjIxLjA1LiAtIDI4LjA2LjIwMDIgV2luZG93cyBOVCA0LjAgVGVjaG5pc2NoZXMgS25vd0hvdyAxLTQ8L2xpPgo8bGk+MjEuMDUuIC0gMjguMDYuMjAwMiBFaW5mJnV1bWw7aHJ1bmcgaW4gTmV0endlcmt0ZWNobm9sb2dpZSAxLTI8L2xpPgo8bGk+MTMuMDIuIC0gMjIuMDUuMjAwMSBFdXJvcCZhdW1sO2lzY2hlciBDb21wdXRlcmYmdXVtbDtocmVyc2NoZWluIChFQ0RMKTwvbGk+CjxsaT4wNS4wMy4gLSAwNi4wNC4yMDAxIEF1cy0gdW5kIFdlaXRlcmJpbGR1bmcgaW0gVmVya2F1ZjwvbGk+CjwvdWw+Cjxocj4KPGg0PkFVU0JJTERVTkc8L2g0Pgo8dWw+CjxsaT4xOTg0IC0gMTk4OCBWb2xrc3NjaHVsZTwvbGk+CjxsaT4xOTg4IC0gMTk5MiBIYXVwdHNjaHVsZTwvbGk+CjxsaT4xOTkyIC0gMTk5MyBQb2x5dGVjaG5pc2NoZXIgTGVocmdhbmcgKE1pdCBBdXN6ZWljaG51bmcpPC9saT4KPGxpPjE5OTMgLSAxOTk3IExhbmRlc2JlcnVmc3NjaHVsZSAoTWl0IEF1c3plaWNobnVuZyk8L2xpPgo8L3VsPgo8aHI+CjxoND5JTlRFUkVTU0VOPC9oND4KPHVsPgo8bGk+TXVzaWssIEtpbm8sIDxhIGhyZWY9Imh0dHA6Ly93d3cua2FyYW9rZS13aWVuLmF0LyIgdGFyZ2V0PSJfYmxhbmsiIHRpdGxlPSJCYWJ1ZGVyJnJzcXVvO3MgfCBNYWNoIGRpZSBXZWx0IHp1IGRlaW5lciBCJnV1bWw7aG5lIHwgS2FyYW9rZSBpbiBXaWVuOiI+S2FyYW9rZTwvYT4KPC9saT4KPGxpPlNwb3J0IHNvZmVybmUgZXMgZGllIEZyZWl6ZWl0IHVuZCBkYXMgV2V0dGVyIHp1bGFzc2VuPC9saT4KPGxpPkVEViB1bmQgSW5mb3JtYXRpb25zdGVjaG5vbG9naWUgYXVjaCBwcml2YXQ8L2xpPgo8bGk+VW50ZXJoYWx0dW5nZWxla3Ryb25payBqZWdsaWNoZXIgQXJ0PC9saT4KPGxpPlNvZnR3YXJlLUVudHdpY2tsdW5nIG1pdCBWaXN1YWwgQmFzaWMgNi4wIHp1ciBwcml2YXRlbiBWZXJ3ZW5kdW5nPC9saT4KPGxpPlRlc3RlbiB2b24gU3lzdGVtZW4gdW5kIE5ldHplcmtlbiBpbSBwcml2YXRlbiB1bmQgZ2VzY2gmYXVtbDtmdGxpY2hlbiBVbWZlbGQ8L2xpPgo8bGk+SW50ZXJuZXQtUHJvZ3JhbW1pZXJ1bmcgKEphdmFTY3JpcHQgLyBDU1MgLyBQSFAgLyBNeVNRTCk8L2xpPgo8bGk+QXVkaW8tIHVuZCBCaWxkYmVhcmJlaXR1bmc8L2xpPgo8L3VsPgo8aHI+CjxoND5NVVNJSzwvaDQ+Cjx1bD4KPGxpPkhlYXZ5LU1ldGFsLCBSb2NrLCBLdXNjaGVsLVJvY2ssIE9sZGllcywgQXVzdHJvLVBvcDwvbGk+CjxsaT5EZWVwIFB1cnBsZSwgSm9lIEx5bm4gVHVybmVyLCBJYW4gR2lsbGFuLCBEZWYgTGVwcGFyZCwgQWVyb3NtaXRoLCBBQy9EQywgRG9ybywgQiZvdW1sO2hzZSBPbmtlbHosIE1ldGFsbGljYSwgR3VucyBOJmFjdXRlOyBSb3NlcywgSnVkYXMgUHJpZXN0LCBIZWxsb3dlZW4sIEtJU1MsIEFsaWNlIENvb3BlciwgQmxhY2sgU2FiYmF0aCwgTWFub3dhciwgTWFnbnVtLCBTYXZhdGFnZSwgVmljdG9yeSwgTGVkIFplcHBlbGluLCBHbGVubiBIdWdoZXMsIE51IFBhZ2FkaTwvbGk+CjxsaT5Kb2FuYSBaaW1tZXIsIFBldGVyIENldGVyYSwgQnJ5YW4gQWRhbXMsIFRvdG8sIFF1ZWVuLCBSRU0sIEV1cm9wZSwgU2NvcnBpb25zLCBXaGl0ZSBMaW9uLCBNZWF0IExvYWYsIEJvbiBKb3ZpLCBEaXJlIFN0cmFpdHMsIFJveGV0dGUsIENoaWNhZ28sIFNhbnRhbmEsIFN0YXR1cyBRdW8sIFN1cnZpdm9yLCBGb3JlaWduZXIsIEJvc3RvbiwgQm9uZmlyZSwgUmFpbmJvdywgR2VuZXNpcywgUG9pc29uLCBKb2huIE5vcnVtLCBSYWVtb25uPC9saT4KPGxpPlNUUywgSGVyYmVydCBHciZvdW1sO25lbXllciwgT3B1cywgRmFsY28sIFRvdGVuIEhvc2VuLCBXb2xmZ2FuZyBBbWJyb3MsIEEzLCBQZXRlciBNYWZmYXksIEtsYXVzIExhZ2UsIFB1ciwgUGV0ZXIgQ29ybmVsaXVzLCBIYW5zaSBEdWptaWM8L2xpPgo8L3VsPgo8aHI+CjxoND5TQ0hBVVNQSUVMRVIgLyBGSUxNRSAvIFNFUklFTjwvaDQ+Cjx1bD4KPGxpPkplYW4gQ2xhdWRlIFZhbiBEYW1tZSwgU3lsdmVzdGVyIFN0YWxsb25lLCBTdGV2ZW4gU2VhZ2FsLCBMb3JlbnpvIExhbWFzLCBOaWNvbGFzIENhZ2UsIFJpY2hhcmQgRGVhbiBBbmRlcnNvbiwgRGVuemVsIFdhc2hpbmd0b24sIENocmlzdG9waGVyIExhbWJlcnQsIE1lZyBSeWFuLCBTYXJhaCBNaWNoZWxsZSBHZWxsYXIsIEFtYW5kYSBUYXBwaW5nIC4uLi48L2xpPgo8bGk+SyZvdW1sO25pZ3JlaWNoIGRlciBIaW1tZWwsIEhlcnIgZGVyIFJpbmdlLCBIaWdobGFuZGVyLCBDb24gQWlyLCBUaGUgUm9jaywgU3RhcmdhdGUsIEluZGVwZW5kZW5jZSBEYXksIFp1bSB0Jm91bWw7dGVuIGZyZWlnZWdlYmVuLCBIYXJkIFRvIEtpbGwsIFRoZSBQYXRyaW90LCBSYW1ibywgUm9ja3ksIEhhcnRlIFppZWxlLCBUaW1lY29wLCBCZXN0IE9mIFRoZSBCZXN0LCBCcnVjZSBMZWUgU3RvcnkgLi4uLjwvbGk+CjxsaT5TdGFyZ2F0ZSwgQW5kcm9tZWRhLCBIaWdod2F5IFRvIEhlbGwsIFJlbmVnYWRlLCBOaWtpdGEsIFByb2ZpbGVyLCBDU0ksIERhcmsgQW5nZWwsIEJ1ZmZ5LCBDaGFybWVkPC9saT4KPC91bD4KPC9kaXY+CjwvZGl2Pgo=');
$after_load = (new rh_rte_helper_debug)->on_load($test_string);

var_dump($after_load === $test_string);

final class rh_rte_helper_debug
{
 public $errors = [];

 public function on_load(string $content): string
 {
  $content = utf8_encode($content);
  $content = $this->add_outer_html($content);
  $content = $this->remove_garbage($content);
  $has_root_block = $this->has_root_block($content);
  if(!$has_root_block)
  {
   $content = $this->add_root_block($content);
  }
  $content = $this->remove_outer_html($content);
  $content = mb_convert_encoding($content, 'html-entities', 'UTF-8');
  $content = utf8_decode($content);
  return $content;
 }

 private function has_root_block(string $content): bool
 {
  $return = $content;
  $doc = $this->get_dom($content);
  if($doc != FALSE)
  {
   $xpath = ($doc != FALSE) ? new DomXpath($doc) : FALSE;
   if($xpath != FALSE)
   {
    $path = '//*[contains(concat(" ", normalize-space(@class), " "), "tinymce-generated-root-block")]';
    $nodes = $xpath->query($path);
    $nodes_idx = ($nodes != FALSE) ? $nodes->length : 0;
    if($nodes_idx > 0)
    {
     return TRUE;
    }
    else
    {
     return FALSE;
    }
   }
   else
   {
    return FALSE;
   }
  }
  else
  {
   return FALSE;
  }
 }

 private function add_root_block(string $content): string
 {
  $return = $content;
  $doc = $this->get_dom($content);
  if($doc != FALSE)
  {
   $xpath = ($doc != FALSE) ? new DomXpath($doc) : FALSE;
   if($xpath != FALSE)
   {
    $path = '//*[contains(concat(" ", normalize-space(@class), " "), "tinymce-generated-root-block")]';
    $nodes = $xpath->query($path);
    $nodes_idx = ($nodes != FALSE) ? $nodes->length : 0;
    if($nodes_idx == 0)
    {
     $root_div = $doc->createElement('div');
     $root_div->setAttribute('class', 'tinymce-generated-root-block');
     $root_div->setAttribute('style', 'margin: 0px; padding: 0px;');
     $body = $doc->getElementsByTagName('body')->item(0);
     if($body !== NULL)
     {
      while($body->childNodes->length > 0)
      {
       $root_div->appendChild($body->childNodes->item(0));
      }
      $body->appendChild($root_div);
      $return = $doc->saveHTML();
     }
    }
   }
  }
  return $return;
 }

 private function add_outer_html(string $content): string
 {
  return '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd"><html><head><title>Tidy</title></head><body>' . $content . '</body></html>';
 }

 private function remove_root_block(string $content): string
 {
  return $this->remove_block($content, 'tinymce-generated-root-block');
 }

 private function remove_garbage(string $content): string
 {
  return $this->remove_block($content, 'tinymce-garbage-root-block');
 }

 private function get_dom(string $html, bool $add_outer_html=TRUE): DOMDocument
 {
  $use_internal_errors      = libxml_use_internal_errors(TRUE);
  $dom                      = new DOMDocument;
  $dom->resolveExternals    = FALSE;
  $dom->preserveWhiteSpace  = TRUE;
  $dom->strictErrorChecking = FALSE;
  $dom->formatOutput        = TRUE;
  $dom->recover             = TRUE;
  $dom->validateOnParse     = TRUE;
  $dom->substituteEntities  = FALSE;
  $html                     = mb_convert_encoding($html, 'HTML-ENTITIES', 'UTF-8');
  $options = 0;
  if(!$add_outer_html)
  {
   $options = LIBXML_HTML_NOIMPLIED;
  }
  $options = $options | LIBXML_HTML_NODEFDTD;
  $loaded = @$dom->loadHTML('<?xml encoding="UTF-8">' . $html, $options);
  if(!$loaded)
  {
   $dom = FALSE;
   $this->errors = libxml_get_errors();
  }
  else
  {
   foreach($dom->childNodes as $item)
   {
    if($item->nodeType == XML_PI_NODE)
    {
     $dom->removeChild($item);
    }
   }
   $dom->encoding = 'UTF-8';
  }
  libxml_clear_errors();
  libxml_use_internal_errors($use_internal_errors);
  return $dom;
 }

 private function remove_outer_html(string $content): string
 {
  $return = $content;
  $doc = $this->get_dom($content);
  if($doc !== FALSE)
  {
   if($doc->doctype !== NULL)
   {
    $doc->doctype->parentNode->removeChild($doc->doctype);
   }
   $html = $doc->getElementsByTagName('html')->item(0);
   if($html !== NULL)
   {
    $fragment = $doc->createDocumentFragment();
    while($html->childNodes->length > 0)
    {
     $childNode = $html->childNodes->item(0);
     $fragment->appendChild($childNode);
    }
    $html->parentNode->replaceChild($fragment, $html);
   }
   $body = $doc->getElementsByTagName('body')->item(0);
   if($body !== NULL)
   {
    $return = '';
    $fragment = $doc->createDocumentFragment();
    while($body->childNodes->length > 0)
    {
     $childNode = $body->childNodes->item(0);
     $fragment->appendChild($childNode);
     $return .= $doc->saveHTML($childNode);
    }
    $body->parentNode->replaceChild($fragment, $body);
   }
   else
   {
    $return = $doc->saveHTML();
   }
  }
  return $return;
 }

 private function remove_block(string $content, string $class='tinymce-generated-root-block'): string
 {
  $return = $content;
  $doc = $this->get_dom($content);
  if($doc != FALSE)
  {
   $xpath = ($doc != FALSE) ? new DomXpath($doc) : FALSE;
   if($xpath != FALSE)
   {
    $path = '//*[contains(concat(" ", normalize-space(@class), " "), "'.$class.'")]';
    $nodes = $xpath->query($path);
    $nodes_idx = ($nodes != FALSE) ? $nodes->length : 0;
    if($nodes_idx > 0)
    {
     foreach($nodes as $node)
     {
      $fragment = $doc->createDocumentFragment();
      while($node->childNodes->length > 0)
      {
       $childNode = $node->childNodes->item(0);
       if($childNode->nodeType == XML_TEXT_NODE)
       {
        $fragment->appendChild($doc->createTextNode($childNode->nodeValue));
        $childNode->parentNode->removeChild($childNode);
       }
       else
       {
        $fragment->appendChild($childNode);
       }
      }
      $node->parentNode->replaceChild($fragment, $node);
     }
     $return = $doc->saveHTML();
    }
   }
  }
  return $return;
 }
}
--EXPECT--
bool(true)
