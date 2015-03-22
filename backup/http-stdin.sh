#!/bin/bash

# this is the location of all the files
webroot="."

# read in the http request from stdin
read cmd request protocol

# print the request to stderr
echo "$cmd $request $protocol" >&2

# urls have lots of weird characters in them like "%20" instead of spaces
# this function converts the weird encoding into normal ascii encoding
# input is stdin, output is stdout
function urldecode() {
    local url_encoded="${1//+/ }"
    printf '%b' "${url_encoded//%/\x}"
}

# the GET request is the most common http command
if [ "$cmd" = "GET" ]; then

    file=$webroot$(cut -d'?' -f1 <<< "$request")
    args=$(cut -d'?' -f2 <<< "$request")
	# if no arguments are passed to cgi, set args null.
    if [ $file = $webroot$args ];then
    	args=""
    fi
    # this is a Common Gateway Interface (CGI) webpage
    #check if requested file is a directory
    if [ -d "$file" ]; then
        if [ -f "./$file/index.html" ]; then
            #display index.html
            info=$(cat "$file/index.html")

            echo "HTTP/1.1 200 OK"
            echo "Content-length: ${#info}"
            echo ""
            echo "$info"

        else
            #print out each file in the directory
            for entry in "$file"/*; do
                echo "$entry"
            done
        fi
    #if requested file isn't a directory
    else
        info=$(cat "$file")
        # check if the file exists in order to use it
        if [ $? = 0 ]; then

            # if the file is an executable, we run it
            if [ -x "$file" ]; then
                #$file $(tr '&' ' ' <<< "$args")
                for arg in $(tr '&' ' ' <<< "$args"); do
                    arg=$(urldecode "$arg")
                    export "$arg"
                    echo "$arg" >&2
                done
				#store the output of executable to temp, then check its content type. We cut file name, only store content type.
				$file > temp
				CONTENT_TYPE=$(file --mime-type temp |cut -d' ' -f2)
				info=$($file)
				rm temp
		
                echo "HTTP/1.1 200 OK"
				echo "Content-type: $CONTENT_TYPE"
                echo "Content-length: ${#info}"
                echo ""
                echo "$info"

            # otherwise, we print it to stdout
            else
                #check content-type via mime-type
                CONTENT_TYPE=$(file --mime-type "$file"|cut -d' ' -f2)

                echo "HTTP/1.1 200 OK"
                echo "Content-type: $CONTENT_TYPE"
                echo "Content-length: ${#info}"
                echo ""
 
                #Its important that you 'cat' the file, because if it is echo'd, the
                #binary data can get mangled.
                cat $file
            fi

        # the file could not be accessed
        else
            # This implements the 404 error page and checks if 404.html exists in the webroot directory.
            if [ ! -e "$file" ]; then
                info=$(cat "$webroot/404.html")
                # If the provided 404 page exists, we will display that page.
                if [ $? = 0 ]; then
                    echo "HTTP/1.1 404 Not Found"
                    echo "Content-length: ${#info}"
                    echo ""
                    echo "$info"
                # Else, if the provided 404 page does not exist, we will display a standard error page.
                else
                    info="
                        <html>
                          <head>
                            <title>404 Not Found</title>
                          </head>
		                  <body>
		                    <h1>Not Found</h1>
			                <p>The requested URL $file was not found on this server.</p>
			              </body>
                       </html>
                        "
                    echo "HTTP/1.1 404 Not Found"
                    echo "Content-length:${#info}"
                    echo ""
                    echo "$info"
                fi
            # This section of code handles 403: Forbidden Errors.
            elif ! [ -r "$file" ]; then
                echo "HTTP/1.1 403 Forbidden"
                error403=$(cat "$webroot/403.html")
                # If $webroot/403.html exists, display that file.
                if [ $? = 0 ]; then
                    echo "Content-length: ${#error403}"
                    echo ""
                    echo "$error403"
                # Otherwise, display a default 403 error page.			
                else
                    info="
                        <html>
                          <head>
                            <title>
                              403 Error: Forbidden
                            </title>
                          </head>
                          <body>
                            403 Error: Forbidden. You don't have permission to access $file on this server.
                          </body>
                        </html>
                        "
                    echo "Content-length: ${#info}"
                    echo ""
                    echo $info
                fi
            fi
        fi
    fi

# the POST request is also a valid http command
elif [ "$cmd" = "POST" ]; then
	#read POST request header and get message body length
	while read -r line; do
		[ "$line" == $'\r' ] && break;
		if [[ "$line" =~ ^Content-Length ]];then
			cont_len=`echo -e "$line" |tr -d '\r' | cut -d: -f2`
		fi
	done	
	#read POST request body
	[ "$cont_len" -ne 0 ] && read -n $cont_len body	
	#get date for POST message body
	export "foo=$(cut -d'&' -f1 <<<"$body"|cut -d"=" -f2)"
	export "bar=$(cut -d'&' -f2 <<<"$body"|cut -d"=" -f2)"	
	# check content type of output of executable
	file=./test.sh
	$file > temp
	CONTENT_TYPE=$(file --mime-type temp |cut -d' ' -f2)
	info=$($file)
	rm temp
	#display data sent by POST method
	echo "HTTP/1.1 200 OK"
	echo "Content-Type: $CONTENT_TYPE"
    echo "Content-length: ${#info}"
    echo ""
    echo "$info"
fi
