#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct 		//������ṹ�� �C ÿ��ģ����ص�������й����õĽṹ��
{
    ngx_str_t output_words;
} ngx_http_hello_world_loc_conf_t;

static char* ngx_http_hello_world(ngx_conf_t* cf, ngx_command_t* cmd, void* conf);

static void* ngx_http_hello_world_create_loc_conf(ngx_conf_t* cf);

static char* ngx_http_hello_world_merge_loc_conf(ngx_conf_t* cf, void* parent, void* child);

static ngx_command_t ngx_http_hello_world_commands[] = 
{	//commands�ṹ�壬�趨�������ض��Ĵ�����ʽ������ô��������ʱ�Ĵ�������
    {
	//�����������
	ngx_string("hello_world"), 
	//NGX_HTTP_LOC_CONF��ʾָ����λ�����ò��ֳ����ǺϷ��� ��NGX_CONF_TAKE1: ָ�����һ������
        NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	//������ָ������ʱ���õĺ���	
        ngx_http_hello_world, 
	//�洢λ�ã����ýṹ��+offsetָʾ�����ľ���洢λ��
        NGX_HTTP_LOC_CONF_OFFSET,
        offsetof(ngx_http_hello_world_loc_conf_t, output_words),
        NULL
    },
    ngx_null_command
};

//ʵ��ngx_http_module_t�ӿڣ�������httpģ����������http��ܳ�ʼ��ʱ������ø�ģ�鶨��Ļص�����
static ngx_http_module_t ngx_http_hello_world_module_ctx = 
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    ngx_http_hello_world_create_loc_conf,
    ngx_http_hello_world_merge_loc_conf
};

//����ngx_module_tģ�飬��Ҫ����������ǰ�涨���ngx_http_hello_world_module_ctx��ngx_http_hello_world_commands
//�������е�ctx��commands��Ա�������и�ֵ
ngx_module_t ngx_http_hello_world_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_world_module_ctx,
    ngx_http_hello_world_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

//��������Ļص�����
static ngx_int_t ngx_http_hello_world_handler(ngx_http_request_t* r) {
    ngx_int_t rc;
    ngx_buf_t* b;
    ngx_chain_t out[2];

    ngx_http_hello_world_loc_conf_t* hlcf;
    hlcf = ngx_http_get_module_loc_conf(r, ngx_http_hello_world_module);

    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char*)"text/plain";

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    out[0].buf = b;
    out[0].next = &out[1];
    //����ngx_buf_t,ֱ��ָ��ԭ�ڴ��ַ���������ݽ��и��ƣ���ʡ�ڴ�
    b->pos = (u_char*)"Hello World";
    b->last = b->pos + sizeof("Hello World") - 1;
    b->memory = 1;

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    out[1].buf = b;
    out[1].next = NULL;

    b->pos = hlcf->output_words.data;
    b->last = hlcf->output_words.data + (hlcf->output_words.len);
    b->memory = 1;
    b->last_buf = 1;

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = hlcf->output_words.len + sizeof("hello_world, ") - 1;
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    return ngx_http_output_filter(r, &out[0]);
}

static void* ngx_http_hello_world_create_loc_conf(ngx_conf_t* cf) {
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

static char* ngx_http_hello_world(ngx_conf_t* cf, ngx_command_t* cmd, void* conf) 
{
	ngx_http_core_loc_conf_t* clcf;
	clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    	//��ngx_http_core_loc_conf_t�ṹ��Ļص�������Ϊngx_http_hello_world_handle��
 	//��NGX_HTTP_CONTENT_PHASE�׶Σ�����������������URI�����������ڵ����ÿ���ƥ��ʱ���͵��øûص�����
    	clcf->handler = ngx_http_hello_world_handler;	
    	ngx_conf_set_str_slot(cf, cmd, conf);
    	return NGX_CONF_OK;
}