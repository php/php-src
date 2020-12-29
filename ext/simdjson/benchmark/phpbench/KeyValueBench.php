<?php

declare(strict_types=1);

namespace SimdjsonBench;

use JsonParser;
use PhpBench\Benchmark\Metadata\Annotations\Subject;
use RuntimeException;

use function json_decode;

if (!extension_loaded("simdjson")) {
    die("simdjson must be loaded");
}

/**
 * @Revs(5)
 * @Iterations(5)
 * @Warmup(3)
 * @OutputTimeUnit("milliseconds", precision=5)
 * @BeforeMethods({"init"})
 * @Groups({"key_value"})
 */
class KeyValueBench
{
    /**
     * @var string
     */
    private $json;

    public function init(): void
    {
        $this->json = <<<EOF
{
  "result" : [
    {
      "_key" : "70614",
      "_id" : "products/70614",
      "_rev" : "_al3hU1K---",
      "Hello3" : "World3"
    },
    {
      "_key" : "70616",
      "_id" : "products/70616",
      "_rev" : "_al3hU1K--A",
      "Hello4" : "World4"
    }
  ],
  "hasMore" : false,
  "count" : 2,
  "cached" : false,
  "extra" : {
    "stats" : {
      "writesExecuted" : 0,
      "writesIgnored" : 0,
      "scannedFull" : 4,
      "scannedIndex" : 0,
      "filtered" : 0,
      "httpRequests" : 0,
      "executionTime" : 0.00014734268188476562,
      "peakMemoryUsage" : 2558
    },
    "warnings" : [ ]
  },
  "error" : false,
  "code" : 201
}
EOF;
    }

    /**
     * @Subject()
     */
    public function jsonDecode(): void
    {
        $data = json_decode($this->json, true);

        if ('World3' !== $data['result'][0]['Hello3']) {
            throw new RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonDeepString()
    {
        $value = JsonParser::getKeyValue($this->json, "result/0/Hello3", false);

        if ('World3' !== $value) {
            throw new RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonDeepStringAssoc()
    {
        $value = JsonParser::getKeyValue($this->json, "result/0/Hello3", true);

        if ('World3' !== $value) {
            throw new RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonInt()
    {
        $value = JsonParser::getKeyValue($this->json, "code", false);

        if (201 !== $value) {
            throw new RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonIntAssoc()
    {
        $value = JsonParser::getKeyValue($this->json, "code", true);

        if (201 !== $value) {
            throw new RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonArray()
    {
        $value = JsonParser::getKeyValue($this->json, "result", true);

        if ('World3' !== $value[0]['Hello3']) {
            throw new RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonObject()
    {
        $value = JsonParser::getKeyValue($this->json, "result", false);

        if ('World3' !== $value[0]->Hello3) {
            throw new RuntimeException('error');
        }
    }
}
