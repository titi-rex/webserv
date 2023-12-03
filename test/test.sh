#!/bin/bash

curl --http1.1 -Z http://localhost/[1-50]:8080 --output "test_#1.log"