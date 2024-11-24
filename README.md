*We finally understand why an url starts with HTTP*


# Webserv

![score](https://img.shields.io/badge/Score-110%2F100-ff69b4?style=for-the-badge&logo=42&logoColor=white)

A simple web server from scratch, based on [`nginx`](https://nginx.org/en/). You can setup multiple virtual server, root requested inside filesystem, use `CGI` and server a static website.
All configuration is done via a configuration file. 


## Installation

```bash
  git clone https://github.com/titi-rex/webserv.git 
  cd webserv
  make
  ./webserv [file]
```

If started without argument, webserv will use `conf/example.conf` file as default, which start server with a minimal configuration on port `8080`.
You may need to tweaks the `dir_prefix` directive.


## Features

- Serve a fully static website
- Execute CGI based on file extension
- Handle multiple CGI
- Support file upload
- Autoindexing
- Default error page
- Handle `GET`, `HEAD`, `POST` and `DELETE`
- HTTP redirection
- Can listen on multiple port
- Fully non-blocking without multithreading


## Configuration

Configuration directive, one directive by line except for `server` and `location` block , words after `#` are ignored 

For example see `conf/example.conf`

`dir_prefix`: prefix to use for all path in configuration file

`body_size_limit`: limit client request body size (default :`1024`, max : `*`) 

`error_page`: use a custom error page for a specific code


`server` : define a (virtual) server block directive, you can define as much server as you want, even using the same `host:port`. Server will be differentiated based on their name, first server is default server for an `host:port`

`name` : server names (default: "default")

`host:port` : which host and port to listen to


`dir_cgi`: path where cgi bin are located, must be set before other cgi directives 

`path_cgi`: cgi to execute base on a specific extension

`cgi_available`: indicate which cgi are listed in `dir_cgi`, binary MUST be named as the extension

`location`: define a location block, location route requested ressource to their location on the server file system

`root`: root of the `location` (default: "data", can be used in `server`)

`index`: index to use (default: "index.html", can be used in `server`)

`allow_method`: restrict authorized method on this location

`autoindex`: enable/disable autoindexing

`upload_dir`: directory to save uploaded file (default: disabled)

`return`: HTTP redirection


## Authors

- [@Julien Moutoussamy](https://github.com/jmoutous)
- [@Louisa4124](https://www.github.com/Louisa4124)
- [@titirex](https://www.github.com/titi-rex)

