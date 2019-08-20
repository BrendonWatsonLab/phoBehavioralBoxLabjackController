#include "WebServer.h"
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

///* Exit flag for main loop */
//volatile bool shouldExitNow = false;
//
//class ExampleHandler : public CivetHandler
//{
//public:
//	bool
//		handleGet(CivetServer* server, struct mg_connection* conn)
//	{
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: "
//			"text/html\r\nConnection: close\r\n\r\n");
//		mg_printf(conn, "<html><body>\r\n");
//		mg_printf(conn,
//			"<h2>This is an example text from a C++ handler</h2>\r\n");
//		mg_printf(conn,
//			"<p>To see a page from the A handler <a "
//			"href=\"a\">click here</a></p>\r\n");
//		mg_printf(conn,
//			"<form action=\"a\" method=\"get\">"
//			"To see a page from the A handler with a parameter "
//			"<input type=\"submit\" value=\"click here\" "
//			"name=\"param\" \\> (GET)</form>\r\n");
//		mg_printf(conn,
//			"<form action=\"a\" method=\"post\">"
//			"To see a page from the A handler with a parameter "
//			"<input type=\"submit\" value=\"click here\" "
//			"name=\"param\" \\> (POST)</form>\r\n");
//		mg_printf(conn,
//			"<p>To see a page from the A/B handler <a "
//			"href=\"a/b\">click here</a></p>\r\n");
//		mg_printf(conn,
//			"<p>To see a page from the *.foo handler <a "
//			"href=\"xy.foo\">click here</a></p>\r\n");
//		mg_printf(conn,
//			"<p>To see a page from the WebSocket handler <a "
//			"href=\"ws\">click here</a></p>\r\n");
//		mg_printf(conn,
//			"<p>To exit <a href=\"%s\">click here</a></p>\r\n",
//			EXIT_URI);
//		mg_printf(conn, "</body></html>\r\n");
//		return true;
//	}
//};
//
//class ExitHandler : public CivetHandler
//{
//public:
//	bool
//		handleGet(CivetServer* server, struct mg_connection* conn)
//	{
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: "
//			"text/plain\r\nConnection: close\r\n\r\n");
//		mg_printf(conn, "Bye!\n");
//		shouldExitNow = true;
//		return true;
//	}
//};
//
//class AHandler : public CivetHandler
//{
//private:
//	bool
//		handleAll(const char* method,
//			CivetServer* server,
//			struct mg_connection* conn)
//	{
//		std::string s = "";
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: "
//			"text/html\r\nConnection: close\r\n\r\n");
//		mg_printf(conn, "<html><body>");
//		mg_printf(conn, "<h2>This is the A handler for \"%s\" !</h2>", method);
//		if (CivetServer::getParam(conn, "param", s)) {
//			mg_printf(conn, "<p>param set to %s</p>", s.c_str());
//		}
//		else {
//			mg_printf(conn, "<p>param not set</p>");
//		}
//		mg_printf(conn, "</body></html>\n");
//		return true;
//	}
//
//public:
//	bool
//		handleGet(CivetServer* server, struct mg_connection* conn)
//	{
//		return handleAll("GET", server, conn);
//	}
//	bool
//		handlePost(CivetServer* server, struct mg_connection* conn)
//	{
//		return handleAll("POST", server, conn);
//	}
//};
//
//class ABHandler : public CivetHandler
//{
//public:
//	bool
//		handleGet(CivetServer* server, struct mg_connection* conn)
//	{
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: "
//			"text/html\r\nConnection: close\r\n\r\n");
//		mg_printf(conn, "<html><body>");
//		mg_printf(conn, "<h2>This is the AB handler!!!</h2>");
//		mg_printf(conn, "</body></html>\n");
//		return true;
//	}
//};
//
//class FooHandler : public CivetHandler
//{
//public:
//	bool
//		handleGet(CivetServer* server, struct mg_connection* conn)
//	{
//		/* Handler may access the request info using mg_get_request_info */
//		const struct mg_request_info* req_info = mg_get_request_info(conn);
//
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: "
//			"text/html\r\nConnection: close\r\n\r\n");
//
//		mg_printf(conn, "<html><body>\n");
//		mg_printf(conn, "<h2>This is the Foo GET handler!!!</h2>\n");
//		mg_printf(conn,
//			"<p>The request was:<br><pre>%s %s HTTP/%s</pre></p>\n",
//			req_info->request_method,
//			req_info->request_uri,
//			req_info->http_version);
//		mg_printf(conn, "</body></html>\n");
//
//		return true;
//	}
//	bool
//		handlePost(CivetServer* server, struct mg_connection* conn)
//	{
//		/* Handler may access the request info using mg_get_request_info */
//		const struct mg_request_info* req_info = mg_get_request_info(conn);
//		long long rlen, wlen;
//		long long nlen = 0;
//		long long tlen = req_info->content_length;
//		char buf[1024];
//
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: "
//			"text/html\r\nConnection: close\r\n\r\n");
//
//		mg_printf(conn, "<html><body>\n");
//		mg_printf(conn, "<h2>This is the Foo POST handler!!!</h2>\n");
//		mg_printf(conn,
//			"<p>The request was:<br><pre>%s %s HTTP/%s</pre></p>\n",
//			req_info->request_method,
//			req_info->request_uri,
//			req_info->http_version);
//		mg_printf(conn, "<p>Content Length: %li</p>\n", (long)tlen);
//		mg_printf(conn, "<pre>\n");
//
//		while (nlen < tlen) {
//			rlen = tlen - nlen;
//			if (rlen > sizeof(buf)) {
//				rlen = sizeof(buf);
//			}
//			rlen = mg_read(conn, buf, (size_t)rlen);
//			if (rlen <= 0) {
//				break;
//			}
//			wlen = mg_write(conn, buf, (size_t)rlen);
//			if (wlen != rlen) {
//				break;
//			}
//			nlen += wlen;
//		}
//
//		mg_printf(conn, "\n</pre>\n");
//		mg_printf(conn, "</body></html>\n");
//
//		return true;
//	}
//
//#define fopen_recursive fopen
//
//	bool
//		handlePut(CivetServer* server, struct mg_connection* conn)
//	{
//		/* Handler may access the request info using mg_get_request_info */
//		const struct mg_request_info* req_info = mg_get_request_info(conn);
//		long long rlen, wlen;
//		long long nlen = 0;
//		long long tlen = req_info->content_length;
//		FILE* f;
//		char buf[1024];
//		int fail = 0;
//
//#ifdef _WIN32
//		_snprintf(buf, sizeof(buf), "D:\\somewhere\\%s\\%s", req_info->remote_user, req_info->local_uri);
//		buf[sizeof(buf) - 1] = 0;
//		if (strlen(buf) > 255) {
//			/* Windows will not work with path > 260 (MAX_PATH), unless we use
//			 * the unicode API. However, this is just an example code: A real
//			 * code will probably never store anything to D:\\somewhere and
//			 * must be adapted to the specific needs anyhow. */
//			fail = 1;
//			f = NULL;
//		}
//		else {
//			f = fopen_recursive(buf, "wb");
//		}
//#else
//		snprintf(buf, sizeof(buf), "~/somewhere/%s/%s", req_info->remote_user, req_info->local_uri);
//		buf[sizeof(buf) - 1] = 0;
//		if (strlen(buf) > 1020) {
//			/* The string is too long and probably truncated. Make sure an
//			 * UTF-8 string is never truncated between the UTF-8 code bytes.
//			 * This example code must be adapted to the specific needs. */
//			fail = 1;
//			f = NULL;
//		}
//		else {
//			f = fopen_recursive(buf, "w");
//		}
//#endif
//
//		if (!f) {
//			fail = 1;
//		}
//		else {
//			while (nlen < tlen) {
//				rlen = tlen - nlen;
//				if (rlen > sizeof(buf)) {
//					rlen = sizeof(buf);
//				}
//				rlen = mg_read(conn, buf, (size_t)rlen);
//				if (rlen <= 0) {
//					fail = 1;
//					break;
//				}
//				wlen = fwrite(buf, 1, (size_t)rlen, f);
//				if (wlen != rlen) {
//					fail = 1;
//					break;
//				}
//				nlen += wlen;
//			}
//			fclose(f);
//		}
//
//		if (fail) {
//			mg_printf(conn,
//				"HTTP/1.1 409 Conflict\r\n"
//				"Content-Type: text/plain\r\n"
//				"Connection: close\r\n\r\n");
//		}
//		else {
//			mg_printf(conn,
//				"HTTP/1.1 201 Created\r\n"
//				"Content-Type: text/plain\r\n"
//				"Connection: close\r\n\r\n");
//		}
//
//		return true;
//	}
//};
//
//class WsStartHandler : public CivetHandler
//{
//public:
//	bool
//		handleGet(CivetServer* server, struct mg_connection* conn)
//	{
//
//		mg_printf(conn,
//			"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: "
//			"close\r\n\r\n");
//
//		mg_printf(conn, "<!DOCTYPE html>\n");
//		mg_printf(conn, "<html>\n<head>\n");
//		mg_printf(conn, "<meta charset=\"UTF-8\">\n");
//		mg_printf(conn, "<title>Embedded websocket example</title>\n");
//
//#ifdef USE_WEBSOCKET
//		/* mg_printf(conn, "<script type=\"text/javascript\"><![CDATA[\n"); ...
//		 * xhtml style */
//		mg_printf(conn, "<script>\n");
//		mg_printf(
//			conn,
//			"var i=0\n"
//			"function load() {\n"
//			"  var wsproto = (location.protocol === 'https:') ? 'wss:' : 'ws:';\n"
//			"  connection = new WebSocket(wsproto + '//' + window.location.host + "
//			"'/websocket');\n"
//			"  websock_text_field = "
//			"document.getElementById('websock_text_field');\n"
//			"  connection.onmessage = function (e) {\n"
//			"    websock_text_field.innerHTML=e.data;\n"
//			"    i=i+1;"
//			"    connection.send(i);\n"
//			"  }\n"
//			"  connection.onerror = function (error) {\n"
//			"    alert('WebSocket error');\n"
//			"    connection.close();\n"
//			"  }\n"
//			"}\n");
//		/* mg_printf(conn, "]]></script>\n"); ... xhtml style */
//		mg_printf(conn, "</script>\n");
//		mg_printf(conn, "</head>\n<body onload=\"load()\">\n");
//		mg_printf(
//			conn,
//			"<div id='websock_text_field'>No websocket connection yet</div>\n");
//#else
//		mg_printf(conn, "</head>\n<body>\n");
//		mg_printf(conn, "Example not compiled with USE_WEBSOCKET\n");
//#endif
//		mg_printf(conn, "</body>\n</html>\n");
//
//		return 1;
//	}
//};


/*
 * A simple hello world application class which demonstrates how to react
 * to events, read input, and give feed-back.
 */
class HelloApplication : public Wt::WApplication
{
public:
	HelloApplication(const Wt::WEnvironment& env);

private:
	Wt::WLineEdit* nameEdit_;
	Wt::WText* greeting_;

	void greet();
};

/*
 * The env argument contains information about the new session, and
 * the initial request. It must be passed to the WApplication
 * constructor so it is typically also an argument for your custom
 * application constructor.
*/
HelloApplication::HelloApplication(const Wt::WEnvironment& env)
	: WApplication(env)
{
	setTitle("Hello world");                            // application title

	root()->addWidget(Wt::cpp14::make_unique<Wt::WText>("Your name, please ? ")); // show some text

	nameEdit_ = root()->addWidget(Wt::cpp14::make_unique<Wt::WLineEdit>()); // allow text input
	nameEdit_->setFocus();                              // give focus

	auto button = root()->addWidget(Wt::cpp14::make_unique<Wt::WPushButton>("Greet me."));
	// create a button
	button->setMargin(5, Wt::Side::Left);                   // add 5 pixels margin

	root()->addWidget(Wt::cpp14::make_unique<Wt::WBreak>());    // insert a line break
	greeting_ = root()->addWidget(Wt::cpp14::make_unique<Wt::WText>()); // empty text

	/*
	 * Connect signals with slots
	 *
	 * - simple Wt-way: specify object and method
	 */
	button->clicked().connect(this, &HelloApplication::greet);

	/*
	 * - using an arbitrary function object, e.g. useful to bind
	 *   values with std::bind() to the resulting method call
	 */
	nameEdit_->enterPressed().connect(std::bind(&HelloApplication::greet, this));

	/*
	 * - using a lambda:
	 */
	button->clicked().connect([=]() {
		std::cerr << "Hello there, " << nameEdit_->text() << std::endl;
	});
}

void HelloApplication::greet()
{
	/*
	 * Update the text, using text input into the nameEdit_ field.
	 */
	greeting_->setText("Hello there, " + nameEdit_->text());
}

int runServer(int argc, char** argv)
{
	/*
	 * Your main method may set up some shared resources, but should then
	 * start the server application (FastCGI or httpd) that starts listening
	 * for requests, and handles all of the application life cycles.
	 *
	 * The last argument to WRun specifies the function that will instantiate
	 * new application objects. That function is executed when a new user surfs
	 * to the Wt application, and after the library has negotiated browser
	 * support. The function should return a newly instantiated application
	 * object.
	 */
	// argc: 7
	// argv: "G:\Google Drive\Modern Behavior Box\Code\Labjack\phoBehavioralBoxLabjackController\x64\Debug\phoBehavioralBoxLabjackController.exe"
	//char runPath[] = "G:\\Google Drive\\Modern Behavior Box\\Code\\Labjack\\phoBehavioralBoxLabjackController\\x64\\Debug\\phoBehavioralBoxLabjackController.exe";
	//char* runPathPointer = runPath;
	////char** runPath = (char*[]){"G:\\Google Drive\\Modern Behavior Box\\Code\\Labjack\\phoBehavioralBoxLabjackController\\x64\\Debug\\phoBehavioralBoxLabjackController.exe"};
	//char** runPathPointerPointer = &runPathPointer;

	//int argCount = 7;
	return Wt::WRun(argc, argv, [](const Wt::WEnvironment& env) {
	//return Wt::WRun(argCount, runPathPointerPointer, [](const Wt::WEnvironment& env) {
		/*
		 * You could read information from the environment to decide whether
		 * the user has permission to start a new application
		 */
		return Wt::cpp14::make_unique<HelloApplication>(env);
	});
}

//
//void runServer()
//{
//
//
//	Wt::WServer()
//
//	const char* options[] = {
//		"document_root", DOCUMENT_ROOT, "listening_ports", PORT, 0 };
//
//	std::vector<std::string> cpp_options;
//	for (int i = 0; i < (sizeof(options) / sizeof(options[0]) - 1); i++) {
//		cpp_options.push_back(options[i]);
//	}
//
//	// CivetServer server(options); // <-- C style start
//	CivetServer server(cpp_options); // <-- C++ style start
//
//	ExampleHandler h_ex;
//	server.addHandler(EXAMPLE_URI, h_ex);
//
//	ExitHandler h_exit;
//	server.addHandler(EXIT_URI, h_exit);
//
//	AHandler h_a;
//	server.addHandler("/a", h_a);
//
//	ABHandler h_ab;
//	server.addHandler("/a/b", h_ab);
//
//	WsStartHandler h_ws;
//	server.addHandler("/ws", h_ws);
//
//#ifdef NO_FILES
//	/* This handler will handle "everything else", including
//	 * requests to files. If this handler is installed,
//	 * NO_FILES should be set. */
//	FooHandler h_foo;
//	server.addHandler("", h_foo);
//
//	printf("See a page from the \"all\" handler at http://localhost:%s/\n", PORT);
//#else
//	FooHandler h_foo;
//	server.addHandler("**.foo", h_foo);
//	printf("Browse files at http://localhost:%s/\n", PORT);
//#endif
//
//#ifdef USE_WEBSOCKET
//	WebSocketHandler h_websocket;
//	server.addWebSocketHandler("/websocket", h_websocket);
//	printf("Run websocket example at http://localhost:%s/ws\n", PORT);
//#endif
//
//	printf("Run example at http://localhost:%s%s\n", PORT, EXAMPLE_URI);
//	printf("Exit at http://localhost:%s%s\n", PORT, EXIT_URI);
//
////	while (!shouldExitNow) {
////#ifdef _WIN32
////		Sleep(1000);
////#else
////		sleep(1);
////#endif
////	}
////
////	printf("Bye!\n");
//}
