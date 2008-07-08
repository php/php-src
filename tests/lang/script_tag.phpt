--TEST--
<script> tag
--FILE--
<script language=php> echo "ola\n";</script>
<script language="php"> echo "ola2\n";</script>
<script language='php'> echo "ola3\n";</script>
texto <sc <s <script> <script language> <script language=>
<script language=php>
#comment
echo "oi\n"; //ignore here
# 2nd comment
--EXPECT--
ola
ola2
ola3
texto <sc <s <script> <script language> <script language=>
oi
