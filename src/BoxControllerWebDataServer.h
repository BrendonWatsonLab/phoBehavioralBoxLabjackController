#pragma once
#include <Wt/WSignal.h>
#include <Wt/WString.h>

#include <set>
#include <map>
#include <thread>
#include <mutex>


namespace Wt {
	class WServer;
}

/*! \brief Encapsulate a chat event.
 */
class DataServerEvent
{
public:
	/*! \brief Enumeration for the event type.
	 */
	enum Type { Login, Logout, Rename, Message };

	/*! \brief Get the event type.
	 */
	Type type() const { return type_; }

	/*! \brief Get the user who caused the event.
	 */
	const Wt::WString& user() const { return user_; }

	/*! \brief Get the message of the event.
	 */
	const Wt::WString& message() const { return message_; }

	/*! \brief Get the extra data for this event.
	 */
	const Wt::WString& data() const { return data_; }

	/*! \brief Get the message formatted as HTML, rendered for the given user.
	 *
	 * The \p format indicates how the message should be formatted.
	 */
	const Wt::WString formattedHTML(const Wt::WString& user,
		Wt::TextFormat format) const;

private:
	Type    type_;
	Wt::WString user_;
	Wt::WString data_;
	Wt::WString message_;

	/*
	 * Both user and html will be formatted as html
	 */
	DataServerEvent(const Wt::WString& user, const Wt::WString& message)
		: type_(Message), user_(user), message_(message)
	{ }

	DataServerEvent(Type type, const Wt::WString& user,
		const Wt::WString& data = Wt::WString::Empty)
		: type_(type), user_(user), data_(data)
	{ }

	friend class BoxControllerWebDataServer;
};

typedef std::function<void(const DataServerEvent&)> DataServerEventCallback;



//BoxControllerWebDataServer
class BoxControllerWebDataServer
{
public:
	/*
	 * A reference to a client.
	 */
	class Client
	{
	};

	/*! \brief Create a new chat server.
	 */
	BoxControllerWebDataServer(Wt::WServer& server);

	BoxControllerWebDataServer(const BoxControllerWebDataServer&) = delete;
	BoxControllerWebDataServer& operator=(const BoxControllerWebDataServer&) = delete;

	/*! \brief Connects to the data server.
	 *
	 * The passed callback method is posted to when a new data server event is
	 * received.
	 *
	 * Returns whether the client has been connected (or false if the client
	 * was already connected).
	 */
	bool connect(Client* client, const DataServerEventCallback& handleEvent);

	/*! \brief Disconnect from the chat server.
	 *
	 * Returns whether the client has been disconnected (or false if the client
	 * was not connected).
	 */
	bool disconnect(Client* client);

	///*! \brief Try to login with given user name.
	// *
	// * Returns false if the login was not successful.
	// */
	//bool login(const Wt::WString& user);

	///*! \brief Logout from the server.
	// */
	//void logout(const Wt::WString& user);

	///*! \brief Changes the name.
	// */
	//bool changeName(const Wt::WString& user, const Wt::WString& newUser);

	///*! \brief Get a suggestion for a guest user name.
	// */
	//Wt::WString suggestGuest();

	///*! \brief Send a message on behalve of a user.
	// */
	//void sendMessage(const Wt::WString& user, const Wt::WString& message);

	/*! \brief Typedef for a collection of user names.
	 */
	typedef std::set<Wt::WString> UserSet;

	/*! \brief Get the users currently logged in.
	 */
	UserSet users();

private:
	struct ClientInfo {
		std::string       sessionId;
		DataServerEventCallback eventCallback;
	};

	typedef std::map<Client*, ClientInfo> ClientMap;

	Wt::WServer& server_;
	std::recursive_mutex    mutex_;
	ClientMap               clients_;
	UserSet                 users_;

	void postDataServerEvent(const DataServerEvent& event);
};

