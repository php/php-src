<html>
<body bgcolor="white">
<head>
<title>Cookies Example</title>
</head>
<body>
<h3>Cookies Example</h3>

<?
  $cookies = $request->cookies;
 
  // print out cookies

  if (!current($cookies)) {
    echo "Your browser isn't sending any cookies\n";
  } else {
    echo "Your browser is sending the following cookies:<br>\n";

    foreach ($cookies as $cookie) {
      echo "Cookie Name: $cookie->name<br>Cookie value: $cookie->value<br>\n";
    }
  }

  // set a cookie

  if ($cookieName) {
    $response->addCookie(new Java("javax.servlet.http.Cookie", $cookieName, $cookieValue));
    echo "<p>You just sent the following cookie to your browser:<br>\n";
    echo "Name: $cookieName<br>Value: $cookieValue<P>\n";
  }

?>
<P>
Create a cookie to send to your browser<br>
<form action="<?PHP echo $PHP_SELF ?>" method=POST>
Name:  <input type=text length=20 name=cookieName><br>
Value:  <input type=text length=20 name=cookieValue><br>
<input type=submit></form>
</body>
</html>

