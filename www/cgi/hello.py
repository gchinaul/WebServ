#!/usr/bin/env python3
"""
Very small, portable CGI test script.
Works on macOS and Linux (Ubuntu). Prints minimal HTML and a few CGI env vars.
"""
import os
import sys
from datetime import datetime

# Print CGI header
sys.stdout.write("Content-Type: text/html\r\n\r\n")

now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

html = [
    '<!DOCTYPE html>',
    '<html lang="fr">',
    '<head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">',
    '<title>CGI Test</title>',
    '<style>body{font-family:Arial,Helvetica,sans-serif;margin:20px;background:#f7f7f7;color:#111}h1{color:#2b7}pre{background:#eee;padding:10px;border-radius:6px}</style>',
    '</head>',
    '<body>',
    '<h1>CGI Python — OK</h1>',
    '<p>Timestamp: %s</p>' % now,
    '<h2>Environment (selected)</h2>',
    '<pre>'
]

keys = ['REQUEST_METHOD','QUERY_STRING','CONTENT_TYPE','CONTENT_LENGTH','SCRIPT_NAME','PATH_INFO','SERVER_NAME','SERVER_PORT','REMOTE_ADDR']
for k in keys:
    html.append('%s = %s' % (k, os.environ.get(k, '')))

html += [
    '</pre>',
    '<p><a href="/">Retour</a></p>',
    '</body>',
    '</html>'
]

sys.stdout.write('\n'.join(html))
sys.stdout.flush()