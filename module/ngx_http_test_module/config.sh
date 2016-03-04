#!/bin/bash

#在config时使用，模块的名称
ngx_addon_name=ngx_http_mytest_module
#在源文件中模块的名称
HTTP_MODULES="$HTTP_MODULES ngx_http_mytest_module"
#源文件的路径
NGX_ADDON_SRCS="$NGX_ADDON_SRCS $ngx_addon_dir/ngx_http_ngx_module.c"