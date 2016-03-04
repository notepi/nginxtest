#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
typedef struct 										//������ṹ�壬ÿ��ģ����ص�������й����õĽṹ��
{
    ngx_str_t output_words;
} ngx_http_hello_world_loc_conf_t;

/*������Ĳ������͸�http���*/
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


#define HTTP_BODY_ON																// ע����رս��հ���

#ifdef HTTP_BODY_ON
static void ngx_http_mytest_body_handler(ngx_http_request_t *r)
{
	/*��ӡ��������*/
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "HTTP BODAY:%s\n", r->request_body->buf->pos);
	/*����ͷ�а���ĳ���*/
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "HTTP BODAYlength:%d\n", r->headers_in.content_length_n);
	/*������ʵ�İ��峤��*/
	ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "HTTP BODAYle:%d\n", r->request_body->buf->last - r->request_body->buf->pos);
	
	ngx_http_finalize_request(r, ngx_http_send_header(r));

	return;
}
#endif



/*ģ�鴦����*/

static ngx_int_t ngx_http_mytest_handler(ngx_http_request_t *r)
{
	// ����ͷ��
	if(!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD | NGX_HTTP_POST)))
	{
		return NGX_HTTP_NOT_ALLOWED;
	}
	
	ngx_log_error(NGX_LOG_DEBUG, r->connection->log, 0, "HEAD REACH13!\n");
	
	#ifndef HTTP_BODY_ON	
	// ��������İ���
	ngx_int_t rc = ngx_http_discard_request_body(r);
	if (rc != NGX_OK)
	{
		return rc;
	}
	#else
	// ���հ���
	ngx_int_t rc = ngx_http_read_client_request_body(r, ngx_http_mytest_body_handler);
	if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
	{
		return rc;
	}

	#endif

	return 	NGX_DONE;
	
	/*���췢������*/
	// ���췢��ͷ
	ngx_str_t type = ngx_string("text/plain");				// ��������
	ngx_str_t response = ngx_string("hello world!");		// ��������
	r->headers_out.status = NGX_HTTP_OK;					// ״ֵ̬
	r->headers_out.content_length_n = response.len;			// �������ݵĳ���
	r->headers_out.content_type = type;						// �������ݵ�����
	
	rc = ngx_http_send_header(r);							// ����ͷ��
	
	ngx_buf_t *b;
	b = ngx_create_temp_buf(r->pool, response.len);			// ���뷢�����ݵ��ڴ棬����ʼ��
	if (b == NULL)
	{
		return NGX_HTTP_INTERNAL_SERVER_ERROR;
	}
	
	ngx_memcpy(b->pos, response.data, response.len);		// ��Ҫ���͵����ݸ��Ƶ��ڴ�
	b->last = b->pos + response.len;
	b->last_buf = 1;										// �����������һ�黺����
	
	// ���췢��ʱ��ngx_chain_t�ṹ��
	ngx_chain_t out;
	out.buf = b;											// Ҫ�������ݳ�ʼ��
	out.next = NULL;										// nextΪ��
	ngx_http_output_filter(r, &out);						// ���Ͱ���
	return 	NGX_DONE;
}

/*����������*/
static char *
ngx_http_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
	ngx_http_core_loc_conf_t *clcf;
	
	/*��ȡģ�������Ԥ����*/
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
	
	/*������ģ�鴦����*/
	clcf->handler = ngx_http_mytest_handler;
	
	/******ȱʧ********/
	ngx_conf_set_str_slot(cf, cmd, conf);					  // ��ngx_str_t�������������

	
	return 	NGX_CONF_OK;
}


// Http���8���׶εĵ��ú���
static ngx_http_module_t ngx_http_mytest_module_ctx = {
	NULL,			// ���������ļ�ǰ����
	NULL,			// ��������ļ��Ľ��������
	
	NULL,			
	NULL,
	
	NULL,
	NULL,
	
	ngx_http_hello_world_create_loc_conf,
	ngx_http_hello_world_merge_loc_conf
};


// ģ����й���Ϣ
static ngx_command_t ngx_http_mytest_commands[] = {
	{
		ngx_string("mytest"),				// ģ������
		/*ģ���ܹ����ֵ�λ��*/
		NGX_HTTP_SRV_CONF	|
		NGX_HTTP_LOC_CONF	|
		//NGX_CONF_NOARGS,					// û�в���
		NGX_CONF_TAKE1,						// һ������
		ngx_http_mytest,					// ģ���г��֡�mytest���󣬽�����õĺ����������õĲ���
		NGX_HTTP_LOC_CONF_OFFSET,			// �������ļ��е�ƫ����
		/*��������ڽṹ�еĳ�Ա�ڽṹ���е�ƫ����*/
		offsetof(ngx_http_hello_world_loc_conf_t, output_words),
		NULL
	},
	ngx_null_command
};




// ģ�����Ҫ����
ngx_module_t ngx_http_mytest_module = {
	/*
	*ctx_index	= 0										// ��ģ���ڱ���ģ�������Ϊ0
	*index		= 0										// ��ģ��������ģ�������Ϊ0
	*spare0 	= 0, spare1 = 0, spare2 = 0, spare3 = 0	// ��������������Ϊ0		
	*version	= 0										// ģ���Ĭ��Ϊ1			
	*/
	NGX_MODULE_V1,										// Ĭ��ֵ
	&ngx_http_mytest_module_ctx,						// Http���8���׶εĵ��ú���
	ngx_http_mytest_commands,							// ģ����й���Ϣ
	NGX_HTTP_MODULE,									// ģ�������
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
	NGX_MODULE_V1_PADDING								// Ĭ��ֵ				
};