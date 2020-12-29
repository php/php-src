<?php

declare(strict_types=1);

namespace SimdjsonBench;

use JsonParser;
use PhpBench\Benchmark\Metadata\Annotations\Subject;

if (!extension_loaded("simdjson")) {
    die("simdjson must be loaded");
}

/**
 * @Revs(5)
 * @Iterations(5)
 * @Warmup(3)
 * @OutputTimeUnit("milliseconds", precision=5)
 * @BeforeMethods({"init"})
 * @Groups({"multiple"})
 */
class MultipleAccessBench
{

    /**
     * @var string
     */
    private $json;

    /**
     * @var string
     */
    private $jsonOther;

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
$this->jsonOther = <<<EOF
{
	"Image": {
		"Width": 800,
		"Height": 600,
		"Title": "View from 15th Floor",
		"Thumbnail": {
			"Url": "http://www.example.com/image/481989943",
			"Height": 125,
			"Width": 100
		},
		"Animated": false,
		"IDs": [116, 943, 234, 38793]
	},
	"Cached": false,
    "HasMore" : true,
    "Count" : 4
}
EOF;
    }

    /**
     * @Subject()
     */
    public function simdjsonMultipleAccessSameDocument()
    {
        $value = JsonParser::getKeyValue($this->json, "result/0/Hello3", true);
        $value2 = JsonParser::getKeyValue($this->json, "code", true);
        $value3 = JsonParser::getKeyValue($this->json, "hasMore", true);
        $value4 = JsonParser::getKeyValue($this->json, "count", true);
        $valueOther = JsonParser::getKeyValue($this->jsonOther, "Image/Width", true);
        $valueOther2 = JsonParser::getKeyValue($this->jsonOther, "Cached", true);
        $valueOther3 = JsonParser::getKeyValue($this->jsonOther, "HasMore", true);
        $valueOther4 = JsonParser::getKeyValue($this->jsonOther, "Count", true);

        if ('World3' !== $value) {
            throw new \RuntimeException('error');
        }
        if (201 !== $value2) {
            throw new \RuntimeException('error');
        }
        if (false !== $value3) {
            throw new \RuntimeException('error');
        }
        if (2 !== $value4) {
            throw new \RuntimeException('error');
        }

        if (800 !== $valueOther) {
            throw new \RuntimeException('error');
        }
        if (false !== $valueOther2) {
            throw new \RuntimeException('error');
        }
        if (true !== $valueOther3) {
            throw new \RuntimeException('error');
        }
        if (4 !== $valueOther4) {
            throw new \RuntimeException('error');
        }
    }

    /**
     * @Subject()
     */
    public function simdjsonMultipleAccessDifferentDocument()
    {
        $value = JsonParser::getKeyValue($this->json, "result/0/Hello3", true);
        $valueOther = JsonParser::getKeyValue($this->jsonOther, "Image/Width", true);
        $value2 = JsonParser::getKeyValue($this->json, "code", true);
        $valueOther2 = JsonParser::getKeyValue($this->jsonOther, "Cached", true);
        $value3 = JsonParser::getKeyValue($this->json, "hasMore", true);
        $valueOther3 = JsonParser::getKeyValue($this->jsonOther, "HasMore", true);
        $value4 = JsonParser::getKeyValue($this->json, "count", true);
        $valueOther4 = JsonParser::getKeyValue($this->jsonOther, "Count", true);

        if ('World3' !== $value) {
            throw new \RuntimeException('error');
        }
        if (201 !== $value2) {
            throw new \RuntimeException('error');
        }
        if (false !== $value3) {
            throw new \RuntimeException('error');
        }
        if (2 !== $value4) {
            throw new \RuntimeException('error');
        }

        if (800 !== $valueOther) {
            throw new \RuntimeException('error');
        }
        if (false !== $valueOther2) {
            throw new \RuntimeException('error');
        }
        if (true !== $valueOther3) {
            throw new \RuntimeException('error');
        }
        if (4 !== $valueOther4) {
            throw new \RuntimeException('error');
        }
    }
}
