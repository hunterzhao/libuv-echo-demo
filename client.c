#include "uv.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

//uv_handle_t > uv_stream_t > uv_tcp_t
static uv_loop_t* loop;
static uv_handle_t* client;
static uv_tcp_t tcpClient;

static void on_close(uv_handle_t* handle);
static void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf);
static void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf);
static void on_write(uv_write_t* req, int status);
static void on_connect(uv_connect_t* connection, int status);
static int tcp4_echo_start(int port);

static void on_close(uv_handle_t* handle)
{
  printf("closed.");
}

static void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) {
    buf->base = malloc(size);
    buf->len = size;
}

static void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
{
  if(nread >= 0) {
    printf("read: %s\n", buf->base);
  }
  else {
    uv_close((uv_handle_t*)tcp, on_close);
  }
  free(buf->base);
}

static void on_write(uv_write_t* req, int status)
{
  printf("wrote.\n");
  //uv_close((uv_handle_t*)req->handle, on_close);
}

static void on_connect(uv_connect_t* connection, int status) {
  printf("connected.\n");
  uv_stream_t* stream = connection->handle;
  uv_buf_t buffer[] = {
    {.base = "hello", .len = 5},
    {.base = "world", .len = 5}
  };
  uv_write_t request;
  uv_write(&request, stream, buffer, 2, on_write);
  uv_read_start(stream, alloc_cb, on_read);
}

static int tcp4_echo_start(int port) {
  struct sockaddr_in addr;
  int r;

  ASSERT(0 == uv_ip4_addr("0.0.0.0", port, &addr));

  client = (uv_handle_t*)&tcpClient;

  r = uv_tcp_init(loop, &tcpClient);
  if (r) {
    /* TODO: Error codes */
    fprintf(stderr, "Socket creation error\n");
    return 1;
  }
  
  uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));
  uv_tcp_connect(connect, &tcpClient,  (const struct sockaddr*) &addr, on_connect);
  return 0;
}


int main() {
  loop = uv_default_loop();

  if (tcp4_echo_start(TEST_PORT))
    return 1;

  uv_run(loop, UV_RUN_DEFAULT);
  return 0;
}