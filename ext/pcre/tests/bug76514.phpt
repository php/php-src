--TEST--
Bug #76514 Regression in preg_match makes it fail with PREG_JIT_STACKLIMIT_ERROR
--FILE--
<?php

$str = '{
    "config": {
        "cache-files-ttl": 0,
        "discard-changes": true
    },
    "minimum-stability": "stable",
    "prefer-stable": false,
    "provide": {
        "heroku-sys/cedar": "14.2016.03.22"
    },
    "repositories": [
        {
            "packagist.org": false
        },
        {
            "type": "package",
            "package": [
                {
                    "type": "metapackage",
                    "name": "anthonymartin/geo-location",
                    "version": "v1.0.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "aws/aws-sdk-php",
                    "version": "3.9.4",
                    "require": {
                        "heroku-sys/php": ">=5.5"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "cloudinary/cloudinary_php",
                    "version": "dev-master",
                    "require": {
                        "heroku-sys/ext-curl": "*",
                        "heroku-sys/ext-json": "*",
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine/annotations",
                    "version": "v1.2.7",
                    "require": {
                        "heroku-sys/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine/cache",
                    "version": "v1.6.0",
                    "require": {
                        "heroku-sys/php": "~5.5|~7.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine/collections",
                    "version": "v1.3.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine/common",
                    "version": "v2.6.1",
                    "require": {
                        "heroku-sys/php": "~5.5|~7.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine/inflector",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine/lexer",
                    "version": "v1.0.1",
                    "require": {
                        "heroku-sys/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "geoip/geoip",
                    "version": "v1.16",
                    "require": [],
                    "replace": [],
                    "provide": [],
                    "conflict": {
                        "heroku-sys/ext-geoip": "*"
                    }
                },
                {
                    "type": "metapackage",
                    "name": "giggsey/libphonenumber-for-php",
                    "version": "7.2.5",
                    "require": {
                        "heroku-sys/ext-mbstring": "*"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp/guzzle",
                    "version": "5.3.0",
                    "require": {
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp/promises",
                    "version": "1.0.3",
                    "require": {
                        "heroku-sys/php": ">=5.5.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp/psr7",
                    "version": "1.2.3",
                    "require": {
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp/ringphp",
                    "version": "1.1.0",
                    "require": {
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp/streams",
                    "version": "3.0.0",
                    "require": {
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "hipchat/hipchat-php",
                    "version": "v1.4",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "kriswallsmith/buzz",
                    "version": "v0.15",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league/csv",
                    "version": "8.0.0",
                    "require": {
                        "heroku-sys/ext-mbstring": "*",
                        "heroku-sys/php": ">=5.5.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league/fractal",
                    "version": "0.13.0",
                    "require": {
                        "heroku-sys/php": ">=5.4"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "mashape/unirest-php",
                    "version": "1.2.1",
                    "require": {
                        "heroku-sys/ext-curl": "*",
                        "heroku-sys/ext-json": "*",
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "mtdowling/jmespath.php",
                    "version": "2.3.0",
                    "require": {
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "palex/phpstructureddata",
                    "version": "v2.0.1",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "psr/http-message",
                    "version": "1.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "react/promise",
                    "version": "v2.2.1",
                    "require": {
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "rollbar/rollbar",
                    "version": "v0.15.0",
                    "require": {
                        "heroku-sys/ext-curl": "*"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "ronanguilloux/isocodes",
                    "version": "1.2.0",
                    "require": {
                        "heroku-sys/ext-bcmath": "*",
                        "heroku-sys/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "sendgrid/sendgrid",
                    "version": "2.1.1",
                    "require": {
                        "heroku-sys/php": ">=5.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "sendgrid/smtpapi",
                    "version": "0.0.1",
                    "require": {
                        "heroku-sys/php": ">=5.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony/css-selector",
                    "version": "v2.8.2",
                    "require": {
                        "heroku-sys/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony/http-foundation",
                    "version": "v2.8.2",
                    "require": {
                        "heroku-sys/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony/polyfill-php54",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony/polyfill-php55",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "thepixeldeveloper/sitemap",
                    "version": "3.0.0",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "tijsverkoyen/css-to-inline-styles",
                    "version": "1.5.5",
                    "require": {
                        "heroku-sys/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "yiisoft/yii",
                    "version": "1.1.17",
                    "require": {
                        "heroku-sys/php": ">=5.1.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "composer.json/composer.lock",
                    "version": "dev-597511d6d51b96e4a8afeba2c79982e5",
                    "require": {
                        "heroku-sys/php": "~5.6.0",
                        "heroku-sys/ext-newrelic": "*",
                        "heroku-sys/ext-gd": "*",
                        "heroku-sys/ext-redis": "*"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                }
            ]
        }
    ],
    "require": {
        "composer.json/composer.lock": "dev-597511d6d51b96e4a8afeba2c79982e5",
        "anthonymartin/geo-location": "v1.0.0",
        "aws/aws-sdk-php": "3.9.4",
        "cloudinary/cloudinary_php": "dev-master",
        "doctrine/annotations": "v1.2.7",
        "doctrine/cache": "v1.6.0",
        "doctrine/collections": "v1.3.0",
        "doctrine/common": "v2.6.1",
        "doctrine/inflector": "v1.1.0",
        "doctrine/lexer": "v1.0.1",
        "geoip/geoip": "v1.16",
        "giggsey/libphonenumber-for-php": "7.2.5",
        "guzzlehttp/guzzle": "5.3.0",
        "guzzlehttp/promises": "1.0.3",
        "guzzlehttp/psr7": "1.2.3",
        "guzzlehttp/ringphp": "1.1.0",
        "guzzlehttp/streams": "3.0.0",
        "hipchat/hipchat-php": "v1.4",
        "kriswallsmith/buzz": "v0.15",
        "league/csv": "8.0.0",
        "league/fractal": "0.13.0",
        "mashape/unirest-php": "1.2.1",
        "mtdowling/jmespath.php": "2.3.0",
        "palex/phpstructureddata": "v2.0.1",
        "psr/http-message": "1.0",
        "react/promise": "v2.2.1",
        "rollbar/rollbar": "v0.15.0",
        "ronanguilloux/isocodes": "1.2.0",
        "sendgrid/sendgrid": "2.1.1",
        "sendgrid/smtpapi": "0.0.1",
        "symfony/css-selector": "v2.8.2",
        "symfony/http-foundation": "v2.8.2",
        "symfony/polyfill-php54": "v1.1.0",
        "symfony/polyfill-php55": "v1.1.0",
        "thepixeldeveloper/sitemap": "3.0.0",
        "tijsverkoyen/css-to-inline-styles": "1.5.5",
        "yiisoft/yii": "1.1.17",
        "heroku-sys/apache": "^2.4.10",
        "heroku-sys/nginx": "~1.8.0"
    }
}';

$res = preg_match('{(?(DEFINE)
       (?<number>   -? (?= [1-9]|0(?!\d) ) \d+ (\.\d+)? ([eE] [+-]? \d+)? )
       (?<boolean>   true | false | null )
       (?<string>    " ([^"\\\\]* | \\\\ ["\\\\bfnrt\/] | \\\\ u [0-9a-f]{4} )* " )
       (?<array>     \[  (?:  (?&json) \s* (?: , (?&json) \s* )*  )?  \s* \] )
       (?<pair>      \s* (?&string) \s* : (?&json) \s* )
       (?<object>    \{  (?:  (?&pair)  (?: , (?&pair)  )*  )?  \s* \} )
       (?<json>   \s* (?: (?&number) | (?&boolean) | (?&string) | (?&array) | (?&object) ) )
    )
^(?P<start>\s*\{\s*(?:(?&string)\s*:\s*(?&json)\s*,\s*)*?)
(?P<property>'.preg_quote('"require"').'\s*:\s*)(?P<value>(?&json))(?P<end>.*)}sx', $str, $match);

var_dump($res, $match['value'] ?? null, preg_last_error() == PREG_JIT_STACKLIMIT_ERROR);
?>
--EXPECT--
int(1)
string(1613) "{
        "composer.json/composer.lock": "dev-597511d6d51b96e4a8afeba2c79982e5",
        "anthonymartin/geo-location": "v1.0.0",
        "aws/aws-sdk-php": "3.9.4",
        "cloudinary/cloudinary_php": "dev-master",
        "doctrine/annotations": "v1.2.7",
        "doctrine/cache": "v1.6.0",
        "doctrine/collections": "v1.3.0",
        "doctrine/common": "v2.6.1",
        "doctrine/inflector": "v1.1.0",
        "doctrine/lexer": "v1.0.1",
        "geoip/geoip": "v1.16",
        "giggsey/libphonenumber-for-php": "7.2.5",
        "guzzlehttp/guzzle": "5.3.0",
        "guzzlehttp/promises": "1.0.3",
        "guzzlehttp/psr7": "1.2.3",
        "guzzlehttp/ringphp": "1.1.0",
        "guzzlehttp/streams": "3.0.0",
        "hipchat/hipchat-php": "v1.4",
        "kriswallsmith/buzz": "v0.15",
        "league/csv": "8.0.0",
        "league/fractal": "0.13.0",
        "mashape/unirest-php": "1.2.1",
        "mtdowling/jmespath.php": "2.3.0",
        "palex/phpstructureddata": "v2.0.1",
        "psr/http-message": "1.0",
        "react/promise": "v2.2.1",
        "rollbar/rollbar": "v0.15.0",
        "ronanguilloux/isocodes": "1.2.0",
        "sendgrid/sendgrid": "2.1.1",
        "sendgrid/smtpapi": "0.0.1",
        "symfony/css-selector": "v2.8.2",
        "symfony/http-foundation": "v2.8.2",
        "symfony/polyfill-php54": "v1.1.0",
        "symfony/polyfill-php55": "v1.1.0",
        "thepixeldeveloper/sitemap": "3.0.0",
        "tijsverkoyen/css-to-inline-styles": "1.5.5",
        "yiisoft/yii": "1.1.17",
        "heroku-sys/apache": "^2.4.10",
        "heroku-sys/nginx": "~1.8.0"
    }"
bool(false)
