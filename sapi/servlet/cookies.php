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

    for ($cookie=current($cookies); $cookie; $cookie=next($cookies)) {
      echo "Cookie Name: $cookie->name<br>Cookie value: $cookie->value<br>\n";
    }
  }

  // set a cookie

  $name = $request->getParameter("cookieName");
  if ($name) {
    $value = $request->getParameter("cookieValue");
    $response->addCookie(new Java("javax.servlet.http.Cookie", $name, $value));
    echo "<p>You just sent the following cookie to your browser:<br>\n";
    echo "Name: $name<br>Value: $value<P>\n";
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

