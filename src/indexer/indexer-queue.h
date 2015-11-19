#ifndef INDEXER_QUEUE_H
#define INDEXER_QUEUE_H

#include "indexer.h"

struct indexer_request {
	struct indexer_request *prev, *next;

	char *username;
	char *mailbox;
	char *session_id;
	unsigned int max_recent_msgs;

	/* index messages in this mailbox */
	unsigned int index:1;
	/* optimize this mailbox */
	unsigned int optimize:1;
	/* currently indexing this mailbox */
	unsigned int working:1;
	/* after indexing is finished, add this request back to the queue and
	   reindex it (i.e. a new indexing request came while we were
	   working.) */
	unsigned int reindex_head:1;
	unsigned int reindex_tail:1;

	/* when working finished, call this number of contexts and leave the
	   rest to the reindexing. */
	unsigned int working_context_idx;

	ARRAY(void *) contexts;
};

struct indexer_queue *indexer_queue_init(indexer_status_callback_t *callback);
void indexer_queue_deinit(struct indexer_queue **queue);

/* The callback is called whenever a new request is added to the queue. */
void indexer_queue_set_listen_callback(struct indexer_queue *queue,
				       void (*callback)(struct indexer_queue *));
	
void indexer_queue_append(struct indexer_queue *queue, bool append,
			  const char *username, const char *mailbox,
			  const char *session_id, unsigned int max_recent_msgs,
			  void *context);
void indexer_queue_append_optimize(struct indexer_queue *queue,
				   const char *username, const char *mailbox,
				   void *context);
void indexer_queue_cancel_all(struct indexer_queue *queue);

bool indexer_queue_is_empty(struct indexer_queue *queue);
unsigned int indexer_queue_count(struct indexer_queue *queue);

/* Return the next request from the queue, without removing it. */
struct indexer_request *indexer_queue_request_peek(struct indexer_queue *queue);
/* Remove the next request from the queue. You must call
   indexer_queue_request_finish() to free its memory. */
void indexer_queue_request_remove(struct indexer_queue *queue);
/* Give a status update about how far the indexing is going on. */
void indexer_queue_request_status(struct indexer_queue *queue,
				  struct indexer_request *request,
				  int percentage);
/* Start working on a request */
void indexer_queue_request_work(struct indexer_request *request);
/* Finish the request and free its memory. */
void indexer_queue_request_finish(struct indexer_queue *queue,
				  struct indexer_request **request,
				  bool success);

#endif
