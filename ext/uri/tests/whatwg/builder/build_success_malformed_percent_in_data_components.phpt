--TEST--
Test Uri\WhatWg\UrlBuilder::build() - success - malformed percent in data components
--FILE--
<?php

foreach ([
    'hierarchical path' => new Uri\WhatWg\UrlBuilder()
        ->setScheme('https')
        ->setHost('example.com')
        ->setPath('a%'),
    'opaque path' => new Uri\WhatWg\UrlBuilder()
        ->setScheme('scheme')
        ->setPath('a%'),
    'query' => new Uri\WhatWg\UrlBuilder()
        ->setScheme('https')
        ->setHost('example.com')
        ->setQuery('a%'),
    'fragment' => new Uri\WhatWg\UrlBuilder()
        ->setScheme('https')
        ->setHost('example.com')
        ->setFragment('a%'),
] as $description => $builder) {

    $errors = [];

    $url = $builder->build(softErrors: $errors);

    echo $description, "\n";
    var_dump($url->toAsciiString());
    var_dump($errors);
    var_dump($url->equals(new Uri\WhatWg\Url($url->toAsciiString()), Uri\UriComparisonMode::IncludeFragment));
}

?>
--EXPECTF--
hierarchical path
string(22) "https://example.com/a%"
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) "%"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
opaque path
string(9) "scheme:a%"
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) "%"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
query
string(23) "https://example.com/?a%"
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) "%"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
fragment
string(23) "https://example.com/#a%"
array(1) {
  [0]=>
  object(Uri\WhatWg\UrlValidationError)#%d (%d) {
    ["context"]=>
    string(1) "%"
    ["type"]=>
    enum(Uri\WhatWg\UrlValidationErrorType::InvalidUrlUnit)
    ["failure"]=>
    bool(false)
  }
}
bool(true)
