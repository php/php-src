--TEST--
GH-25008 (com_event_sink and com_print_typeinfo crash on a NULL source typelib)
--EXTENSIONS--
com_dotnet
--INI--
com.autoregister_typelib=0
--SKIPIF--
<?php
if (!extension_loaded('com_dotnet')) die('skip com_dotnet not loaded');
try {
    new COM('WinHttp.WinHttpRequest.5.1');
    new COM('Scripting.Dictionary');
} catch (com_exception $e) {
    die('skip WinHttp or Scripting.Dictionary is not available');
}
?>
--FILE--
<?php
$o = new COM('WinHttp.WinHttpRequest.5.1', null, CP_UTF8);

try {
    var_dump(com_event_sink($o, new stdClass));
    echo "sink: no error\n";
} catch (Error $e) {
    echo 'sink: ', $e::class, ': ', $e->getMessage(), "\n";
}

ob_start();
$events = com_print_typeinfo($o, null, true);
$events_dump = ob_get_clean();
echo 'events: ';
var_dump($events);
echo (str_contains($events_dump, 'class IWinHttpRequestEvents')
    && str_contains($events_dump, '{F97F4E15-B787-4212-80D1-D380CBBF982E}')
    && str_contains($events_dump, 'function OnResponseStart'))
    ? "events interface\n" : "events missing\n";

ob_start();
$request = com_print_typeinfo($o, null, false);
$request_dump = ob_get_clean();
echo 'request: ';
var_dump($request);
echo (str_contains($request_dump, 'class IWinHttpRequest')
    && str_contains($request_dump, '{016FE2EC-B2C8-45F8-B23B-39E53A75396B}'))
    ? "request interface\n" : "request missing\n";

$d = new COM('Scripting.Dictionary');
echo 'dictionary: ';
var_dump(com_event_sink($d, new stdClass));
?>
--EXPECTF--
sink: Error: Type kind must be dispatchable, 00000003 given
events: bool(true)
events interface
request: bool(true)
request interface
dictionary: 
Warning: com_event_sink(): IProvideClassInfo: this code not yet written! in %s on line %d
bool(false)
