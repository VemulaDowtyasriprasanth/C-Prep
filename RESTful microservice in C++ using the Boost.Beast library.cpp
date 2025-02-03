#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

namespace beast = boost::beast;         // Boost.Beast namespace
namespace http = beast::http;          // HTTP namespace
namespace net = boost::asio;           // Networking namespace
using tcp = boost::asio::ip::tcp;      // TCP namespace

// Function to handle an HTTP session
void handleSession(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;

        // Read the HTTP request
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        // Prepare the HTTP response
        http::response<http::string_body> res{
            http::status::ok, req.version()};

        res.set(http::field::server, "Boost.Beast Example");
        res.set(http::field::content_type, "text/plain");

        if (req.method() == http::verb::get) {
            res.body() = "Hello, this is a simple RESTful microservice!";
        } else {
            res.result(http::status::method_not_allowed);
            res.body() = "Only GET method is supported!";
        }

        res.prepare_payload();

        // Write the response
        http::write(socket, res);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Main function to run the server
int main() {
    try {
        const int port = 8080;
       
        // Create an I/O context
        net::io_context ioc{1};

        // Create an endpoint to listen on
        tcp::acceptor acceptor{ioc, tcp::endpoint(tcp::v4(), port)};
        std::cout << "Server is running on port " << port << std::endl;

        // Run the server loop
        while (true) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);

            // Spawn a thread to handle the session
            std::thread{handleSession, std::move(socket)}.detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
    }
    return 0;
}




/*


Key Features in the Code
Concurrency Handling:

Each HTTP session runs in its own detached thread.
This design avoids blocking the main server loop, allowing it to accept new connections concurrently.
Error Management:

Wrapped handleSession logic in a try-catch block to log exceptions and prevent server crashes during errors (e.g., malformed HTTP requests or client disconnects).
Any critical server errors in the main function are also handled gracefully.
Basic RESTful Functionality:

Handles GET requests and responds with a simple message.
Returns 405 Method Not Allowed for unsupported HTTP methods.
Modularity:

The session logic (handleSession) is independent, making it easy to extend or replace with more complex logic.
Explanation (Interview-Style)
**"In this example, I built a basic RESTful microservice in C++ using Boost.Beast. The server listens on port 8080 and handles each incoming connection in a separate thread, ensuring concurrency. While this approach works for lightweight workloads, for high-performance production systems, I’d use a thread pool or asynchronous operations to optimize resource usage.

Error management is handled with try-catch blocks in both the session handler and the main server loop, ensuring unexpected errors don’t crash the entire server. The service supports basic REST principles by responding to GET requests with a success message and rejecting unsupported methods with a 405 Method Not Allowed response.

In a more robust implementation, I’d add routing for multiple endpoints, support for JSON payloads (e.g., using Boost.JSON), and integrate logging and monitoring tools like Prometheus for production readiness."**



*/