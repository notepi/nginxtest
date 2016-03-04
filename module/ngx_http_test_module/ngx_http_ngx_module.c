#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
typedef struct 										//配置项结构体，每个模块相关的配置项集中管理用的结构体
{
    ngx_str_t output_words;
} ngx_http_hello_world_loc_conf_t;

/*将保存的参数传送给http框架*/
static void* ngx_http_hello_world_create_loc_conf(ngx_conf_t* cf)
 {
    ngx_http_hello_world_loc_conf_t* conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_hello_world_loc_conf_t));
    if (conf == NULL) {
        return NGX_CONF_ERROR;
    }
    conf->output_words.len = 0;
    conf->output_words.data = NULL;

    return conf;
}

static char* ngx_http_hello_world_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child) 
{
    ngx_http_hello_world_loc_conf_t* prev = parent;
    ngx_http_hello_world_loc_conf_t* conf = child;
    ngx_conf_merge_str_value(conf->output_words, prev->output_words, "Nginx");
    return NGX_CONF_OK;
}


#define HTTP_BODY_ON																// 注释则关闭接收包体

#ifdef HTTP_BODY_ON
static void ngx_http_mytest_body_handler(ngx_http_request_t *r)
{
	/*打印包体内容*/
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "HTTP BODAY:%s\n", r->request_body->buf->pos);
	/*报文头中包体的长度*/
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "HTTP BODAYlength:%d\n", r->headers_in.content_length_n);
	/*计算真实的包体长度*/
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "HTTP BODAYle:%d\n", r->request_body->buf->last - r->request_body->buf->pos);
	
	ngx_http_finalize_request(r, ngx_http_send_header(r));

	return;
}
#endif



/*模块处理函数*/

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
	// 处理头部
	if(!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD | NGX_HTTP_POST)))
	{
		return NGX_HTTP_NOT_ALLOWED;
	}
	
	ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "HEAD REACH13!\n");
	
	#ifndef HTTP_BODY_ON	
	// 丢弃请求的包体
	ngx_int_t rc = ngx_http_discard_request_body(r);
	if (rc != NGX_OK)
	{
		return rc;
	}
	#else
	// 接收包体
	ngx_int_t rc = ngx_http_read_client_request_body(r, ngx_http_mytest_body_handler);
	if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
	{
		return rc;
	}

	#endif

	return 	NGX_DONE;
	
	/*构造发送内容*/
	// 构造发送头
	ngx_str_t type = ngx_string("text/plain");				// 包的种类
	ngx_str_t response = ngx_string("hello world!");		// 包的内容
	r->headers_out.status = NGX_HTTP_OK;					// 状态值
	r->headers_out.content_length_n = response.len;			// 包中内容的长度
	r->headers_out.content_type = type;						// 包中内容的种类
	
	rc = ngx_http_send_header(r);							// 发送头部
	
	ngx_buf_t *b;
	b = ngx_create_temp_buf(r->pool, response.len);			// 申请发送数据的内存，并初始化
	if (b == NULL)
	{
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}
	
	ngx_memcpy(b->pos, response.data, response.len);		// 将要发送的内容复制到内存
	b->last = b->pos + response.len;
	b->last_buf = 1;										// 声明这事最后一块缓存区
	
	// 构造发送时的ngx_chain_t结构体
	ngx_chain_t out;
	out.buf = b;											// 要发的内容初始化
	out.next = NULL;										// next为空
	ngx_http_output_filter(r, &out);						// 发送包体
	return 	NGX_DONE;
}

/*参数处理方案*/
static char *
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	
	/*获取模块参数并预处理*/
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	
	/*设置在模块处理函数*/
	clcf->handler = ngx_http_mytest_handler;
	
	/******缺失********/
	ngx_conf_set_str_slot(cf, cmd, conf);					  // 以ngx_str_t类型来保存参数

	
	return 	NGX_CONF_OK;
}


// Http框架8个阶段的调用函数
static ngx_http_module_t ngx_http_mytest_module_ctx = {
	NULL,			// 解析配置文件前调用
	NULL,			// 完成配置文件的解析后调用
	
	NULL,			
	NULL,
	
	NULL,
	NULL,
	
	ngx_http_hello_world_create_loc_conf,
	ngx_http_hello_world_merge_loc_conf
};


// 模块的有关信息
static ngx_command_t ngx_http_mytest_commands[] = {
	{
		ngx_string("mytest"),				// 模块名字
		/*模块能够出现的位置*/
		NGX_HTTP_SRV_CONF	|
		NGX_HTTP_LOC_CONF	|
		//NGX_CONF_NOARGS,					// 没有参数
		NGX_CONF_TAKE1,						// 一个参数
		ngx_http_mytest,					// 模块中出现“mytest”后，将会调用的函数处理配置的参数
		NGX_HTTP_LOC_CONF_OFFSET,			// 在配置文件中的偏移量
		/*参数存放在结构中的成员在结构体中的偏移量*/
		offsetof(ngx_http_hello_world_loc_conf_t, output_words),
		NULL
	},
	ngx_null_command
};




// 模块的主要部分
ngx_module_t ngx_http_mytest_module = {
	/*
	*ctx_index	= 0										// 本模块在本类模块中序号为0
	*index		= 0										// 本模块在所有模块中序号为0
	*spare0 	= 0, spare1 = 0, spare2 = 0, spare3 = 0	// 保留变量，设置为0		
	*version	= 0										// 模块版默认为1			
	*/
	NGX_MODULE_V1,										// 默认值
	&ngx_http_mytest_module_ctx,						// Http框架8个阶段的调用函数
	ngx_http_mytest_commands,							// 模块的有关信息
	NGX_HTTP_MODULE,									// 模块的类型
	NULL,												// init master
	NULL,												// init module
	NULL,												// init process
	NULL,												// init thread
	NULL,												// exit thread
	NULL,												// exit process
	NULL,												// exit master
	/*
	*spare_hook0 = 0, spare_hook1 = 0
	*spare_hook2 = 0, spare_hook3 = 0
	*spare_hook4 = 0, spare_hook5 = 0
	*spare_hook6 = 0, spare_hook7 = 0
	*/
	NGX_MODULE_V1_PADDING								// 默认值				
};