<?php

namespace FPM;

class Status
{
    const HTML_TITLE = 'PHP-FPM Status Page';

    /**
     * @var array
     */
    private $contentTypes = [
        'plain'       => 'text/plain',
        'html'        => 'text/html',
        'xml'         => 'text/xml',
        'json'        => 'application/json',
        'openmetrics' => 'application/openmetrics-text; version=1.0.0; charset=utf-8',
    ];

    /**
     * @var array
     */
    private $defaultFields = [
        'pool'                 => '\w+',
        'process manager'      => '(static|dynamic|ondemand)',
        'start time'           => '\d+\/\w{3}\/\d{4}:\d{2}:\d{2}:\d{2}\s[+-]\d{4}',
        'start since'          => '\d+',
        'accepted conn'        => '\d+',
        'listen queue'         => '\d+',
        'max listen queue'     => '\d+',
        'listen queue len'     => '\d+',
        'idle processes'       => '\d+',
        'active processes'     => '\d+',
        'total processes'      => '\d+',
        'max active processes' => '\d+',
        'max children reached' => '\d+',
        'slow requests'        => '\d+',
    ];

    /**
     * @var Tester
     */
    private Tester $tester;

    /**
     * @param Tester $tester
     */
    public function __construct(Tester $tester)
    {
        $this->tester = $tester;
    }

    /**
     * @param string $body
     * @param string $pattern
     * @return void
     */
    private function matchError(string $body, string $pattern): void
    {
        echo "ERROR: Expected body does not match pattern\n";
        echo "BODY:\n";
        var_dump($body);
        echo "PATTERN:\n";
        var_dump($pattern);
        $this->tester->printLogs();
    }

    /**
     * Check status page.
     *
     * @param Response $response
     * @param array $fields
     * @param string $type
     * @throws \Exception
     */
    public function checkStatus(Response $response, array $fields, string $type)
    {
        if (!isset($this->contentTypes[$type])) {
            throw new \Exception('Invalid content type ' . $type);
        }

        $body = $response->getBody($this->contentTypes[$type]);
        if ($body === null) {
            return;
        }
        $method = "checkStatus" . ucfirst($type);

        $this->$method($body, array_merge($this->defaultFields, $fields));
    }

    /**
     * Make status check for status page.
     *
     * @param string $body
     * @param array $fields
     * @param string $rowPattern
     * @param string $header
     * @param string $footer
     * @param null|callable $nameTransformer
     * @param null|callable $valueTransformer
     * @param bool $startTimeTimestamp
     * @param bool $closingName
     */
    private function makeStatusCheck(
        string $body,
        array $fields,
        string $rowPattern,
        string $header = '',
        string $footer = '',
        $nameTransformer = null,
        $valueTransformer = null,
        bool $startTimeTimestamp = false,
        bool $closingName = false
    ) {

        if ($startTimeTimestamp && $fields['start time'][0] === '\\') {
            $fields['start time'] = '\d+';
        }
        $pattern = '(' . $header;
        foreach ($fields as $name => $value) {
            if ($nameTransformer) {
                $name = call_user_func($nameTransformer, $name);
            }
            if ($valueTransformer) {
                $value = call_user_func($valueTransformer, $value);
            }
            if ($closingName) {
                $pattern .= sprintf($rowPattern, $name, $value, $name);
            } else {
                $pattern .= sprintf($rowPattern, $name, $value);
            }
        }
        $pattern = rtrim($pattern, $rowPattern[strlen($rowPattern) - 1]);
        $pattern .= $footer . ')';

        if (!preg_match($pattern, $body)) {
            $this->matchError($body, $pattern);
        }
    }

    /**
     * Check plain status page.
     *
     * @param string $body
     * @param array $fields
     */
    protected function checkStatusPlain(string $body, array $fields)
    {
        $this->makeStatusCheck($body, $fields, "%s:\s+%s\n");
    }

    /**
     * Check html status page.
     *
     * @param string $body
     * @param array $fields
     */
    protected function checkStatusHtml(string $body, array $fields)
    {
        $header = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" " .
            "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n" .
            "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n" .
            "<head><title>" . self::HTML_TITLE . "</title></head>\n" .
            "<body>\n<table>\n";
        $footer = "\n</table>\n</body></html>";

        $this->makeStatusCheck(
            $body,
            $fields,
            "<tr><th>%s</th><td>%s</td></tr>\n",
            $header,
            $footer
        );
    }

    /**
     * Check xml status page.
     *
     * @param string $body
     * @param array $fields
     */
    protected function checkStatusXml(string $body, array $fields)
    {
        $this->makeStatusCheck(
            $body,
            $fields,
            "<%s>%s</%s>\n",
            "<\?xml version=\"1.0\" \?>\n<status>\n",
            "\n</status>",
            function ($name) {
                return str_replace(' ', '-', $name);
            },
            null,
            true,
            true
        );
    }

    /**
     * Check json status page.
     *
     * @param string $body
     * @param array $fields
     */
    protected function checkStatusJson(string $body, array $fields)
    {
        $this->makeStatusCheck(
            $body,
            $fields,
            '"%s":%s,',
            '{',
            '}',
            null,
            function ($value) {
                if (is_numeric($value) || $value === '\d+') {
                    return $value;
                }

                return '"' . $value . '"';
            },
            true
        );
    }

    /**
     * Check openmetrics status page.
     *
     * @param string $body
     * @param array $fields
     */
    protected function checkStatusOpenmetrics(string $body, array $fields)
    {
        $pattern = "(# HELP phpfpm_up Could pool " . $fields['pool'] . " using a " . $fields['process manager'] . " PM on PHP-FPM be reached\?\n" .
            "# TYPE phpfpm_up gauge\n" .
            "phpfpm_up 1\n" .
            "# HELP phpfpm_start_since The number of seconds since FPM has started\.\n" .
            "# TYPE phpfpm_start_since counter\n" .
            "phpfpm_start_since " . $fields['start since'] . "\n" .
            "# HELP phpfpm_accepted_connections The number of requests accepted by the pool\.\n" .
            "# TYPE phpfpm_accepted_connections counter\n" .
            "phpfpm_accepted_connections " . $fields['accepted conn'] . "\n" .
            "# HELP phpfpm_listen_queue The number of requests in the queue of pending connections\.\n" .
            "# TYPE phpfpm_listen_queue gauge\n" .
            "phpfpm_listen_queue " . $fields['listen queue'] . "\n" .
            "# HELP phpfpm_max_listen_queue The maximum number of requests in the queue of pending connections since FPM has started\.\n" .
            "# TYPE phpfpm_max_listen_queue counter\n" .
            "phpfpm_max_listen_queue " . $fields['max listen queue'] . "\n" .
            "# TYPE phpfpm_listen_queue_length gauge\n" .
            "# HELP phpfpm_listen_queue_length The size of the socket queue of pending connections\.\n" .
            "phpfpm_listen_queue_length " . $fields['listen queue len'] . "\n" .
            "# HELP phpfpm_idle_processes The number of idle processes\.\n" .
            "# TYPE phpfpm_idle_processes gauge\n" .
            "phpfpm_idle_processes " . $fields['idle processes'] . "\n" .
            "# HELP phpfpm_active_processes The number of active processes\.\n" .
            "# TYPE phpfpm_active_processes gauge\n" .
            "phpfpm_active_processes " . $fields['active processes'] . "\n" .
            "# HELP phpfpm_total_processes The number of idle \+ active processes\.\n" .
            "# TYPE phpfpm_total_processes gauge\n" .
            "phpfpm_total_processes " . $fields['total processes'] . "\n" .
            "# HELP phpfpm_max_active_processes The maximum number of active processes since FPM has started\.\n" .
            "# TYPE phpfpm_max_active_processes counter\n" .
            "phpfpm_max_active_processes " . $fields['max active processes'] . "\n" .
            "# HELP phpfpm_max_children_reached The number of times, the process limit has been reached, when pm tries to start more children \(works only for pm 'dynamic' and 'ondemand'\)\.\n" .
            "# TYPE phpfpm_max_children_reached counter\n" .
            "phpfpm_max_children_reached " . $fields['max children reached'] . "\n" .
            "# HELP phpfpm_slow_requests The number of requests that exceeded your 'request_slowlog_timeout' value\.\n" .
            "# TYPE phpfpm_slow_requests counter\n" .
            "phpfpm_slow_requests " . $fields['slow requests'] . "\n" .
            "# EOF)\n";

        if (!preg_match($pattern, $body)) {
            $this->matchError($body, $pattern);
        }
    }
}
