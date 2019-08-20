#include "WebServer.h"
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

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
