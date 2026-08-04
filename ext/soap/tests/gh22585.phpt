--TEST--
GH-22585 (Use of uninitialized params in do_request() on out-of-memory bailout)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
memory_limit=64M
--FILE--
<?php
/* Deep recursion that keeps issuing SOAP calls until memory is exhausted while
 * do_request() is only part-way through initializing its params array. The
 * cleanup path must not touch the uninitialized slots. Depending on the
 * environment the run ends either by the recursion limit (reaching "Done") or
 * by the memory-exhaustion fatal; both are fine, a crash/UB abort is not. */
try {
    class MySoapClient extends SoapClient {
        public function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
            return '';
        }
    }

    function main() {
        for (;;) {
            $soap = new MySoapClient(
                null,
                ['location' => "http://localhost/soap.php", 'uri' => "http://localhost/"]
            );
            $soap->call(1.1);
            main();
        }
    }

    main();
} catch (\Throwable $e) {
}
echo "Done" . PHP_EOL;
?>
--EXPECTREGEX--
(?s)(Done|.*Allowed memory size of \d+ bytes exhausted.*)
