--TEST--
Test password_verify() with bcrypt-sha256 using passlib test vectors (v2 format)
--FILE--
<?php
//-=-=-=-
/*
 * Test vectors from passlib (https://github.com/pydanny/passlib),
 * passlib/tests/test_handlers_bcrypt.py, class _bcrypt_sha256_test.
 *
 * Passlib
 * Copyright (c) 2008-2020 Assurance Technologies, LLC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *   * Neither the name of Assurance Technologies, nor the names of the
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// passlib UPASS_TABLE = u("t\u00e1\u0411\u2113\u0259")
$upass = "t\xc3\xa1\xd0\x91\xe2\x84\x93\xc9\x99";

$good = [
    # empty
    "" => '$bcrypt-sha256$v=2,t=2b,r=5$E/e/2AOhqM5W/KJTFQzLce$WFPIZKtDDTriqWwlmRFfHiOTeheAZWe',

    # ascii
    "password" => '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # unicode / utf8
    $upass => '$bcrypt-sha256$v=2,t=2b,r=5$.US1fQ4TQS.ZTz/uJ5Kyn.$pzzgp40k8reM1CuQb03PvE0IDPQSdV6',

    # test >72 chars is hashed correctly -- under bcrypt these hash the same.
    str_repeat('abc123', 12) => '$bcrypt-sha256$v=2,t=2b,r=5$X1g1nh3g0v4h6970O68cxe$zu1cloESVFIOsUIo7fCEgkdHaI9SSue',
    str_repeat('abc123', 12) . 'qwr' => '$bcrypt-sha256$v=2,t=2b,r=5$X1g1nh3g0v4h6970O68cxe$CBF9csfEdW68xv3DwE6xSULXMtqEFP.',
    str_repeat('abc123', 12) . 'xyz' => '$bcrypt-sha256$v=2,t=2b,r=5$X1g1nh3g0v4h6970O68cxe$zC/1UDUG2ofEXB6Onr2vvyFzfhEOS3S',
];

$bad = [
    # bad char in otherwise correct hash
    #                                   \/
    '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKF!PE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # unsupported version (for this format)
    '$bcrypt-sha256$v=1,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # unrecognized version
    '$bcrypt-sha256$v=3,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # unrecognized bcrypt variant
    '$bcrypt-sha256$v=2,t=2c,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # unsupported bcrypt variant
    '$bcrypt-sha256$v=2,t=2a,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',
    '$bcrypt-sha256$v=2,t=2x,r=5$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # rounds zero-padded
    '$bcrypt-sha256$v=2,t=2b,r=05$5Hg1DKFqPE8C2aflZ5vVoe$wOK1VFFtS8IGTrGa7.h5fs0u84qyPbS',

    # config string w/ $ added
    '$bcrypt-sha256$v=2,t=2b,r=5$5Hg1DKFqPE8C2aflZ5vVoe$',
];

echo "# Good hashes\n";
foreach ($good as $password => $hash) {
    var_dump(password_verify($password, $hash));
    var_dump(password_verify($password . "x", $hash));
}

echo "# Bad hashes\n";
foreach ($bad as $hash) {
    var_dump(password_verify('password', $hash));
}

?>
--EXPECT--
# Good hashes
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
# Bad hashes
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
