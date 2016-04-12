--TEST--
Pattern exhausting PCRE JIT stack
--FILE--
<?php

$re = '{^(\\s*\\{\\s*(?:"(?:[^\\0-\\x09\\x0a-\\x1f\\\\"]+|\\\\["bfnrt/\\\\]|\\\\u[a-fA-F0-9]{4})*"\\s*:\\s*(?:[0-9.]+|null|true|false|"(?:[^\\0-\\x09\\x0a-\\x1f\\\\"]+|\\\\["bfnrt/\\\\]|\\\\u[a-fA-F0-9]{4})*"|\\[(?:[^\\]]*|\\[(?:[^\\]]*|\\[(?:[^\\]]*|\\[(?:[^\\]]*|\\[[^\\]]*\\])*\\])*\\])*\\]|(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{[^{}]*\\})*\\})*\\})*\\})*)*\\]|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{[^{}]*\\})*\\})*\\})*\\})*\\})\\s*,\\s*)*?)("require"\\s*:\\s*)((?:[0-9.]+|null|true|false|"(?:[^\\0-\\x09\\x0a-\\x1f\\\\"]+|\\\\["bfnrt/\\\\]|\\\\u[a-fA-F0-9]{4})*"|\\[(?:[^\\]]*|\\[(?:[^\\]]*|\\[(?:[^\\]]*|\\[(?:[^\\]]*|\\[[^\\]]*\\])*\\])*\\])*\\]|(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{[^{}]*\\})*\\})*\\})*\\})*)*\\]|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{(?:[^{}]*|\\{[^{}]*\\})*\\})*\\})*\\})*\\}))(.*)}s';

$str = '{
    "config": {
        "cache-files-ttl": 0,
        "discard-changes": true
    },
    "minimum-stability": "stable",
    "prefer-stable": false,
    "provide": {
        "heroku-sys\\/cedar": "14.2016.03.12"
    },
    "repositories": [
        {
            "packagist": false
        },
        {
            "type": "path",
            "url": "\\/tmp\\/buildpacktUY7k\\/support\\/installer\\/",
            "options": {
                "symlink": false
            }
        },
        {
            "type": "composer",
            "url": "https:\\/\\/lang-php.s3.amazonaws.com\\/dist-cedar-14-stable\\/"
        },
        {
            "type": "package",
            "package": [
                {
                    "type": "metapackage",
                    "name": "algolia\\/algoliasearch-client-php",
                    "version": "1.8.1",
                    "require": {
                        "heroku-sys\\/ext-mbstring": "*",
                        "heroku-sys\\/php": ">=5.4"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "algolia\\/algoliasearch-laravel",
                    "version": "1.0.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "asm89\\/stack-cors",
                    "version": "0.2.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "aws\\/aws-sdk-php",
                    "version": "3.15.7",
                    "require": {
                        "heroku-sys\\/php": ">=5.5"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "aws\\/aws-sdk-php-laravel",
                    "version": "3.1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "barryvdh\\/laravel-cors",
                    "version": "v0.7.3",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "bookingsync\\/oauth2-bookingsync-php",
                    "version": "0.1.3",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "classpreloader\\/classpreloader",
                    "version": "3.0.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "danielstjules\\/stringy",
                    "version": "1.10.0",
                    "require": {
                        "heroku-sys\\/ext-mbstring": "*",
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "dnoegel\\/php-xdg-base-dir",
                    "version": "0.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/annotations",
                    "version": "v1.2.7",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/cache",
                    "version": "v1.6.0",
                    "require": {
                        "heroku-sys\\/php": "~5.5|~7.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/collections",
                    "version": "v1.3.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/common",
                    "version": "v2.6.1",
                    "require": {
                        "heroku-sys\\/php": "~5.5|~7.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/dbal",
                    "version": "v2.5.4",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/inflector",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "doctrine\\/lexer",
                    "version": "v1.0.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "fabpot\\/goutte",
                    "version": "v3.1.2",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "graham-campbell\\/manager",
                    "version": "v2.3.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzle\\/guzzle",
                    "version": "v3.9.3",
                    "require": {
                        "heroku-sys\\/ext-curl": "*",
                        "heroku-sys\\/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp\\/guzzle",
                    "version": "6.1.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp\\/promises",
                    "version": "1.1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "guzzlehttp\\/psr7",
                    "version": "1.2.3",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "intercom\\/intercom-php",
                    "version": "v1.4.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "intervention\\/image",
                    "version": "2.3.6",
                    "require": {
                        "heroku-sys\\/ext-fileinfo": "*",
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "jakub-onderka\\/php-console-color",
                    "version": "0.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "jakub-onderka\\/php-console-highlighter",
                    "version": "v0.3.2",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "jeremeamia\\/SuperClosure",
                    "version": "2.2.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.4"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "jlapp\\/swaggervel",
                    "version": "dev-master",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "laravel\\/framework",
                    "version": "v5.1.31",
                    "require": {
                        "heroku-sys\\/ext-mbstring": "*",
                        "heroku-sys\\/ext-openssl": "*",
                        "heroku-sys\\/php": ">=5.5.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "laravelcollective\\/html",
                    "version": "v5.1.9",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league\\/flysystem",
                    "version": "1.0.18",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league\\/flysystem-aws-s3-v3",
                    "version": "1.0.9",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league\\/fractal",
                    "version": "0.13.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.4"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league\\/glide",
                    "version": "1.0.0",
                    "require": {
                        "heroku-sys\\/php": "^5.4 | ^7.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "league\\/oauth2-client",
                    "version": "0.12.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "mindscape\\/raygun4php",
                    "version": "dev-master",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "monolog\\/monolog",
                    "version": "1.18.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "mtdowling\\/cron-expression",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "mtdowling\\/jmespath.php",
                    "version": "2.3.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "namshi\\/jose",
                    "version": "5.0.2",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "nesbot\\/carbon",
                    "version": "1.21.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "nikic\\/php-parser",
                    "version": "v2.0.1",
                    "require": {
                        "heroku-sys\\/ext-tokenizer": "*",
                        "heroku-sys\\/php": ">=5.4"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "orangehill\\/iseed",
                    "version": "dev-master",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "paragonie\\/random_compat",
                    "version": "v1.2.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.2.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "phpseclib\\/phpseclib",
                    "version": "0.3.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.0.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "predis\\/predis",
                    "version": "v1.0.3",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "psr\\/http-message",
                    "version": "1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "psy\\/psysh",
                    "version": "v0.7.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "swiftmailer\\/swiftmailer",
                    "version": "v5.4.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/browser-kit",
                    "version": "v2.8.3",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/console",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/css-selector",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/debug",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/dom-crawler",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/event-dispatcher",
                    "version": "v2.8.3",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/finder",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/http-foundation",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/http-kernel",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/polyfill-php56",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/polyfill-util",
                    "version": "v1.1.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.3"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/process",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/routing",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/translation",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "symfony\\/var-dumper",
                    "version": "v2.7.10",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.9"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "twilio\\/sdk",
                    "version": "4.10.0",
                    "require": {
                        "heroku-sys\\/php": ">=5.2.1"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "tymon\\/jwt-auth",
                    "version": "0.5.9",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "vinkla\\/algolia",
                    "version": "2.2.1",
                    "require": {
                        "heroku-sys\\/php": "^5.5.9 || ^7.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "vlucas\\/phpdotenv",
                    "version": "v1.1.1",
                    "require": {
                        "heroku-sys\\/php": ">=5.3.2"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "zircote\\/swagger-php",
                    "version": "2.0.6",
                    "require": {
                        "heroku-sys\\/php": ">=5.4.0"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                },
                {
                    "type": "metapackage",
                    "name": "composer.json\\/composer.lock",
                    "version": "dev-a923f6cdbbc9439cabb74aa9003f6d51",
                    "require": {
                        "heroku-sys\\/php": ">=5.5.9",
                        "heroku-sys\\/ext-gd": "*",
                        "heroku-sys\\/ext-exif": "*",
                        "heroku-sys\\/ext-fileinfo": "*"
                    },
                    "replace": [],
                    "provide": [],
                    "conflict": []
                }
            ]
        }
    ],
    "require": {
        "composer.json\\/composer.lock": "dev-a923f6cdbbc9439cabb74aa9003f6d51",
        "algolia\\/algoliasearch-client-php": "1.8.1",
        "algolia\\/algoliasearch-laravel": "1.0.10",
        "asm89\\/stack-cors": "0.2.1",
        "aws\\/aws-sdk-php": "3.15.7",
        "aws\\/aws-sdk-php-laravel": "3.1.0",
        "barryvdh\\/laravel-cors": "v0.7.3",
        "bookingsync\\/oauth2-bookingsync-php": "0.1.3",
        "classpreloader\\/classpreloader": "3.0.0",
        "danielstjules\\/stringy": "1.10.0",
        "dnoegel\\/php-xdg-base-dir": "0.1",
        "doctrine\\/annotations": "v1.2.7",
        "doctrine\\/cache": "v1.6.0",
        "doctrine\\/collections": "v1.3.0",
        "doctrine\\/common": "v2.6.1",
        "doctrine\\/dbal": "v2.5.4",
        "doctrine\\/inflector": "v1.1.0",
        "doctrine\\/lexer": "v1.0.1",
        "fabpot\\/goutte": "v3.1.2",
        "graham-campbell\\/manager": "v2.3.1",
        "guzzle\\/guzzle": "v3.9.3",
        "guzzlehttp\\/guzzle": "6.1.1",
        "guzzlehttp\\/promises": "1.1.0",
        "guzzlehttp\\/psr7": "1.2.3",
        "intercom\\/intercom-php": "v1.4.0",
        "intervention\\/image": "2.3.6",
        "jakub-onderka\\/php-console-color": "0.1",
        "jakub-onderka\\/php-console-highlighter": "v0.3.2",
        "jeremeamia\\/SuperClosure": "2.2.0",
        "jlapp\\/swaggervel": "dev-master",
        "laravel\\/framework": "v5.1.31",
        "laravelcollective\\/html": "v5.1.9",
        "league\\/flysystem": "1.0.18",
        "league\\/flysystem-aws-s3-v3": "1.0.9",
        "league\\/fractal": "0.13.0",
        "league\\/glide": "1.0.0",
        "league\\/oauth2-client": "0.12.1",
        "mindscape\\/raygun4php": "dev-master",
        "monolog\\/monolog": "1.18.0",
        "mtdowling\\/cron-expression": "v1.1.0",
        "mtdowling\\/jmespath.php": "2.3.0",
        "namshi\\/jose": "5.0.2",
        "nesbot\\/carbon": "1.21.0",
        "nikic\\/php-parser": "v2.0.1",
        "orangehill\\/iseed": "dev-master",
        "paragonie\\/random_compat": "v1.2.1",
        "phpseclib\\/phpseclib": "0.3.10",
        "predis\\/predis": "v1.0.3",
        "psr\\/http-message": "1.0",
        "psy\\/psysh": "v0.7.1",
        "swiftmailer\\/swiftmailer": "v5.4.1",
        "symfony\\/browser-kit": "v2.8.3",
        "symfony\\/console": "v2.7.10",
        "symfony\\/css-selector": "v2.7.10",
        "symfony\\/debug": "v2.7.10",
        "symfony\\/dom-crawler": "v2.7.10",
        "symfony\\/event-dispatcher": "v2.8.3",
        "symfony\\/finder": "v2.7.10",
        "symfony\\/http-foundation": "v2.7.10",
        "symfony\\/http-kernel": "v2.7.10",
        "symfony\\/polyfill-php56": "v1.1.0",
        "symfony\\/polyfill-util": "v1.1.0",
        "symfony\\/process": "v2.7.10",
        "symfony\\/routing": "v2.7.10",
        "symfony\\/translation": "v2.7.10",
        "symfony\\/var-dumper": "v2.7.10",
        "twilio\\/sdk": "4.10.0",
        "tymon\\/jwt-auth": "0.5.9",
        "vinkla\\/algolia": "2.2.1",
        "vlucas\\/phpdotenv": "v1.1.1",
        "zircote\\/swagger-php": "2.0.6",
        "heroku-sys\\/apache": "^2.4.10",
        "heroku-sys\\/nginx": "~1.8.0"
    }
}';

$count = preg_match($re, $str, $matches);

if($count === false) {
    switch (preg_last_error()) {
        case PREG_NO_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_NO_ERROR', PREG_NO_ERROR);
        case PREG_INTERNAL_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_INTERNAL_ERROR', PREG_INTERNAL_ERROR);
        case PREG_BACKTRACK_LIMIT_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_BACKTRACK_LIMIT_ERROR', PREG_BACKTRACK_LIMIT_ERROR);
        case PREG_RECURSION_LIMIT_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_RECURSION_LIMIT_ERROR', PREG_RECURSION_LIMIT_ERROR);
        case PREG_BAD_UTF8_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_BAD_UTF8_ERROR', PREG_BAD_UTF8_ERROR);
        case PREG_BAD_UTF8_OFFSET_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_BAD_UTF8_OFFSET_ERROR', PREG_BAD_UTF8_OFFSET_ERROR);
        case PREG_JIT_STACKLIMIT_ERROR:
            throw new \RuntimeException('Failed to execute regex: PREG_JIT_STACKLIMIT_ERROR', PREG_JIT_STACKLIMIT_ERROR);
        default:
            throw new \RuntimeException('Failed to execute regex: Unknown error');
    }
} else {
	var_dump("Done, $count matches.");
}
?>
==DONE==
--EXPECT--
string(16) "Done, 1 matches."
==DONE==
