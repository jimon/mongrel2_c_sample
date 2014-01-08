TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CFLAGS += -std=c11

SOURCES += main.c

SOURCES += jsmn/jsmn.c
HEADERS += jsmn/jsmn.h

SOURCES += \
	zeromq/clock.cpp\
	zeromq/command.cpp\
	zeromq/connect_session.cpp\
	zeromq/ctx.cpp\
	zeromq/decoder.cpp\
	zeromq/device.cpp\
	zeromq/devpoll.cpp\
	zeromq/dist.cpp\
	zeromq/encoder.cpp\
	zeromq/epoll.cpp\
	zeromq/err.cpp\
	zeromq/fq.cpp\
	zeromq/io_object.cpp\
	zeromq/io_thread.cpp\
	zeromq/ip.cpp\
	zeromq/kqueue.cpp\
	zeromq/lb.cpp\
	zeromq/mailbox.cpp\
	zeromq/named_session.cpp\
	zeromq/object.cpp\
	zeromq/options.cpp\
	zeromq/own.cpp\
	zeromq/pair.cpp\
	zeromq/pgm_receiver.cpp\
	zeromq/pgm_sender.cpp\
	zeromq/pgm_socket.cpp\
	zeromq/pipe.cpp\
	zeromq/poll.cpp\
	zeromq/poller_base.cpp\
	zeromq/pub.cpp\
	zeromq/pull.cpp\
	zeromq/push.cpp\
	zeromq/reaper.cpp\
	zeromq/rep.cpp\
	zeromq/req.cpp\
	zeromq/select.cpp\
	zeromq/session.cpp\
	zeromq/signaler.cpp\
	zeromq/socket_base.cpp\
	zeromq/sub.cpp\
	zeromq/swap.cpp\
	zeromq/tcp_connecter.cpp\
	zeromq/tcp_listener.cpp\
	zeromq/tcp_socket.cpp\
	zeromq/thread.cpp\
	zeromq/transient_session.cpp\
	zeromq/trie.cpp\
	zeromq/uuid.cpp\
	zeromq/xpub.cpp\
	zeromq/xrep.cpp\
	zeromq/xreq.cpp\
	zeromq/xsub.cpp\
	zeromq/zmq.cpp\
	zeromq/zmq_connecter.cpp\
	zeromq/zmq_engine.cpp\
	zeromq/zmq_init.cpp\
	zeromq/zmq_listener.cpp

HEADERS += \
	zeromq/array.hpp\
	zeromq/atomic_counter.hpp\
	zeromq/atomic_ptr.hpp\
	zeromq/blob.hpp\
	zeromq/clock.hpp\
	zeromq/command.hpp\
	zeromq/config.hpp\
	zeromq/connect_session.hpp\
	zeromq/ctx.hpp\
	zeromq/decoder.hpp\
	zeromq/device.hpp\
	zeromq/devpoll.hpp\
	zeromq/dist.hpp\
	zeromq/encoder.hpp\
	zeromq/epoll.hpp\
	zeromq/err.hpp\
	zeromq/fd.hpp\
	zeromq/fq.hpp\
	zeromq/gcc_421_char_traits.hpp\
	zeromq/i_engine.hpp\
	zeromq/i_inout.hpp\
	zeromq/i_poll_events.hpp\
	zeromq/io_object.hpp\
	zeromq/io_thread.hpp\
	zeromq/ip.hpp\
	zeromq/kqueue.hpp\
	zeromq/lb.hpp\
	zeromq/likely.hpp\
	zeromq/mailbox.hpp\
	zeromq/msg_content.hpp\
	zeromq/mutex.hpp\
	zeromq/named_session.hpp\
	zeromq/object.hpp\
	zeromq/options.hpp\
	zeromq/own.hpp\
	zeromq/pair.hpp\
	zeromq/pgm_receiver.hpp\
	zeromq/pgm_sender.hpp\
	zeromq/pgm_socket.hpp\
	zeromq/pipe.hpp\
	zeromq/platform.hpp\
	zeromq/platform.hpp.in\
	zeromq/poll.hpp\
	zeromq/poller.hpp\
	zeromq/poller_base.hpp\
	zeromq/pub.hpp\
	zeromq/pull.hpp\
	zeromq/push.hpp\
	zeromq/reaper.hpp\
	zeromq/rep.hpp\
	zeromq/req.hpp\
	zeromq/select.hpp\
	zeromq/semaphore.hpp\
	zeromq/session.hpp\
	zeromq/signaler.hpp\
	zeromq/socket_base.hpp\
	zeromq/stdint.hpp\
	zeromq/sub.hpp\
	zeromq/swap.hpp\
	zeromq/tcp_connecter.hpp\
	zeromq/tcp_listener.hpp\
	zeromq/tcp_socket.hpp\
	zeromq/thread.hpp\
	zeromq/transient_session.hpp\
	zeromq/trie.hpp\
	zeromq/uuid.hpp\
	zeromq/windows.hpp\
	zeromq/wire.hpp\
	zeromq/xpub.hpp\
	zeromq/xrep.hpp\
	zeromq/xreq.hpp\
	zeromq/xsub.hpp\
	zeromq/ypipe.hpp\
	zeromq/yqueue.hpp\
	zeromq/zmq.h\
	zeromq/zmq.hpp\
	zeromq/zmq_connecter.hpp\
	zeromq/zmq_engine.hpp\
	zeromq/zmq_init.hpp\
	zeromq/zmq_listener.hpp\
	zeromq/zmq_utils.h

INCLUDEPATH += zeromq jsmn

win32::DEFINES += ZMQ_HAVE_WINDOWS
win32::LIBS += libwsock32 libWs2_32 libRpcrt4
