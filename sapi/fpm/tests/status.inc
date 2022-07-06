<?php

namespace FPM;

class Status
{
    const HTML_TITLE = 'PHP-FPM Status Page';

    /**
     * @var array
     */
    private $contentTypes = [
        'plain' => 'text/plain',
        'html'  => 'text/html',
        'xml'   => 'text/xml',
        'json'  => 'application/json',
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
            echo "ERROR: Expected body does not match pattern\n";
            echo "BODY:\n";
            var_dump($body);
            echo "PATTERN:\n";
            var_dump($pattern);
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
}
