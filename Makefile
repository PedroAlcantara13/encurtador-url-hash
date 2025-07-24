all:
	gcc url_shortener.c -o url_shortener.cgi
	chmod +x url_shortener.cgi