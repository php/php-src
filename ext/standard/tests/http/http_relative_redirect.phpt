--TEST--
http wrapper resolves a relative redirect Location against the request path
--DESCRIPTION--
This test tests the current behavior, which is clearly not the correct behavior in some cases.
--SKIPIF--
<?php require_once 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require_once 'server.inc';

$uri_parsers = [null, 'Uri\Rfc3986\Uri', 'Uri\WhatWg\Url'];
$froms = ['/dir/page', '/a', '/other', '', '/../../../foo', '/space bar'];
$tos = ['/dir/page', 'dir/page', 'a', 'other', '', '../../../foo', 'space bar'];

foreach ($uri_parsers as $uri_parser) {
    foreach ($froms as $from) {
        foreach ($tos as $to) {
            $responses[] = "data://text/plain,HTTP/1.0 302 Found\r\nLocation: $to\r\n\r\n";
            $responses[] = "data://text/plain,HTTP/1.0 204 No Content\r\n\r\n";
        }
    }
}

['pid' => $pid, 'uri' => $uri] = http_server($responses, $output);

foreach ($uri_parsers as $uri_parser) {
    echo "# URI parser: $uri_parser\n";
    foreach ($froms as $from) {
        foreach ($tos as $to) {
            ftruncate($output, 0);

            $ctx = stream_context_create(['http' => [
              'follow_location' => true,
              'uri_parser_class' => $uri_parser,
            ]]);
            $body = @file_get_contents($uri . $from, false, $ctx);
            rewind($output);

            if ($body === false) {
                $result = "failed";
                // Remove request from responses queue
                file_get_contents($uri);
            } else {
                $requests = stream_get_contents($output);
                preg_match_all('~GET (.*) HTTP/1.~', $requests, $matches);
                $result = $matches[1][1];
            }
            echo "Redirect from '$from' to '$to': $result\n";
        }
    }
}

http_server_kill($pid);
?>
--EXPECT--
# URI parser: 
Redirect from '/dir/page' to '/dir/page': /dir/page
Redirect from '/dir/page' to 'dir/page': /dir//dir/page
Redirect from '/dir/page' to 'a': /a
Redirect from '/dir/page' to 'other': /dir//other
Redirect from '/dir/page' to '': /
Redirect from '/dir/page' to '../../../foo': /dir//../../../foo
Redirect from '/dir/page' to 'space bar': /dir//space bar
Redirect from '/a' to '/dir/page': /dir/page
Redirect from '/a' to 'dir/page': /dir/page
Redirect from '/a' to 'a': /a
Redirect from '/a' to 'other': /other
Redirect from '/a' to '': /
Redirect from '/a' to '../../../foo': /../../../foo
Redirect from '/a' to 'space bar': /space bar
Redirect from '/other' to '/dir/page': /dir/page
Redirect from '/other' to 'dir/page': /dir/page
Redirect from '/other' to 'a': /a
Redirect from '/other' to 'other': /other
Redirect from '/other' to '': /
Redirect from '/other' to '../../../foo': /../../../foo
Redirect from '/other' to 'space bar': /space bar
Redirect from '' to '/dir/page': /dir/page
Redirect from '' to 'dir/page': /dir/page
Redirect from '' to 'a': /a
Redirect from '' to 'other': /other
Redirect from '' to '': /
Redirect from '' to '../../../foo': /../../../foo
Redirect from '' to 'space bar': /space bar
Redirect from '/../../../foo' to '/dir/page': /dir/page
Redirect from '/../../../foo' to 'dir/page': /../../..//dir/page
Redirect from '/../../../foo' to 'a': /a
Redirect from '/../../../foo' to 'other': /../../..//other
Redirect from '/../../../foo' to '': /
Redirect from '/../../../foo' to '../../../foo': /../../..//../../../foo
Redirect from '/../../../foo' to 'space bar': /../../..//space bar
Redirect from '/space bar' to '/dir/page': /dir/page
Redirect from '/space bar' to 'dir/page': /dir/page
Redirect from '/space bar' to 'a': /a
Redirect from '/space bar' to 'other': /other
Redirect from '/space bar' to '': /
Redirect from '/space bar' to '../../../foo': /../../../foo
Redirect from '/space bar' to 'space bar': /space bar
# URI parser: Uri\Rfc3986\Uri
Redirect from '/dir/page' to '/dir/page': /dir/page
Redirect from '/dir/page' to 'dir/page': /dir//dir/page
Redirect from '/dir/page' to 'a': /a
Redirect from '/dir/page' to 'other': /dir//other
Redirect from '/dir/page' to '': /
Redirect from '/dir/page' to '../../../foo': /dir//../../../foo
Redirect from '/dir/page' to 'space bar': failed
Redirect from '/a' to '/dir/page': /dir/page
Redirect from '/a' to 'dir/page': /dir/page
Redirect from '/a' to 'a': /a
Redirect from '/a' to 'other': /other
Redirect from '/a' to '': /
Redirect from '/a' to '../../../foo': /../../../foo
Redirect from '/a' to 'space bar': failed
Redirect from '/other' to '/dir/page': /dir/page
Redirect from '/other' to 'dir/page': /dir/page
Redirect from '/other' to 'a': /a
Redirect from '/other' to 'other': /other
Redirect from '/other' to '': /
Redirect from '/other' to '../../../foo': /../../../foo
Redirect from '/other' to 'space bar': failed
Redirect from '' to '/dir/page': /dir/page
Redirect from '' to 'dir/page': /dir/page
Redirect from '' to 'a': /a
Redirect from '' to 'other': /other
Redirect from '' to '': /
Redirect from '' to '../../../foo': /../../../foo
Redirect from '' to 'space bar': failed
Redirect from '/../../../foo' to '/dir/page': /dir/page
Redirect from '/../../../foo' to 'dir/page': /../../..//dir/page
Redirect from '/../../../foo' to 'a': /a
Redirect from '/../../../foo' to 'other': /../../..//other
Redirect from '/../../../foo' to '': /
Redirect from '/../../../foo' to '../../../foo': /../../..//../../../foo
Redirect from '/../../../foo' to 'space bar': failed
Redirect from '/space bar' to '/dir/page': failed
Redirect from '/space bar' to 'dir/page': failed
Redirect from '/space bar' to 'a': failed
Redirect from '/space bar' to 'other': failed
Redirect from '/space bar' to '': failed
Redirect from '/space bar' to '../../../foo': failed
Redirect from '/space bar' to 'space bar': failed
# URI parser: Uri\WhatWg\Url
Redirect from '/dir/page' to '/dir/page': /dir/page
Redirect from '/dir/page' to 'dir/page': /dir//dir/page
Redirect from '/dir/page' to 'a': /a
Redirect from '/dir/page' to 'other': /dir//other
Redirect from '/dir/page' to '': /
Redirect from '/dir/page' to '../../../foo': /foo
Redirect from '/dir/page' to 'space bar': /dir//space%20bar
Redirect from '/a' to '/dir/page': /dir/page
Redirect from '/a' to 'dir/page': /dir/page
Redirect from '/a' to 'a': /a
Redirect from '/a' to 'other': /other
Redirect from '/a' to '': /
Redirect from '/a' to '../../../foo': /foo
Redirect from '/a' to 'space bar': /space%20bar
Redirect from '/other' to '/dir/page': /dir/page
Redirect from '/other' to 'dir/page': /dir/page
Redirect from '/other' to 'a': /a
Redirect from '/other' to 'other': /other
Redirect from '/other' to '': /
Redirect from '/other' to '../../../foo': /foo
Redirect from '/other' to 'space bar': /space%20bar
Redirect from '' to '/dir/page': /dir/page
Redirect from '' to 'dir/page': /dir/page
Redirect from '' to 'a': /a
Redirect from '' to 'other': /other
Redirect from '' to '': /
Redirect from '' to '../../../foo': /foo
Redirect from '' to 'space bar': /space%20bar
Redirect from '/../../../foo' to '/dir/page': /dir/page
Redirect from '/../../../foo' to 'dir/page': /dir/page
Redirect from '/../../../foo' to 'a': /a
Redirect from '/../../../foo' to 'other': /other
Redirect from '/../../../foo' to '': /
Redirect from '/../../../foo' to '../../../foo': /foo
Redirect from '/../../../foo' to 'space bar': /space%20bar
Redirect from '/space bar' to '/dir/page': /dir/page
Redirect from '/space bar' to 'dir/page': /dir/page
Redirect from '/space bar' to 'a': /a
Redirect from '/space bar' to 'other': /other
Redirect from '/space bar' to '': /
Redirect from '/space bar' to '../../../foo': /foo
Redirect from '/space bar' to 'space bar': /space%20bar
