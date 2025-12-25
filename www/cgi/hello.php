#!/usr/bin/php-cgi
<?php
header("Content-Type: text/html");
echo "<html><body>";
echo "<h1>Hello from PHP CGI!</h1>";
echo "<p>Request Method: " . $_SERVER['REQUEST_METHOD'] . "</p>";
echo "<p>Query String: " . $_SERVER['QUERY_STRING'] . "</p>";
echo "<p>Current Time: " . date('Y-m-d H:i:s') . "</p>";
echo "</body></html>";
?>
