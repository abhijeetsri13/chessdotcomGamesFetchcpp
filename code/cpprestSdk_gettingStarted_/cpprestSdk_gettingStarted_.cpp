#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams
using namespace boost::filesystem;
using boost::bad_lexical_cast;
int main(int argc, char* argv[])
{
	auto fileStream = std::make_shared<ostream>();
	int styr = 2011;
	int endyr = 2020;
	utility::string_t user;
	// Open stream to output file.
	try {
	if (argc < 2 || argc>4) {
		std::cout << "\nNo Extra Command Line Argument Passed Other Than Program Name \n Ussage code.exe [username] [startYear] [endYear] \n";
		return 0;
	}
	if (argc >= 2)
	{
		std::cout << "\nNumber Of Arguments Passed";
		printf("\n----Following Are The Command Line Arguments Passed----\n");
		user = utility::conversions::to_string_t(argv[1]);
		std::cout << "USERNAME: " << user << std::endl;
		styr = boost::lexical_cast<int>(argv[2]);
		std::cout << "START YEAR: " << styr << std::endl;
		endyr = boost::lexical_cast<int>(argv[3]);
		std::cout << "START YEAR: " << endyr << std::endl;
	}




	for (int year = styr; year <= endyr; year++) {
		for (int month = 1; month <= 12; month++)
		{
			std::string yr = std::to_string(year);
			std::string mon = std::to_string(month);
			std::string path = utility::conversions::to_utf8string(user);
			if (!boost::filesystem::exists(path))
				boost::filesystem::create_directory(path);

			utility::string_t filePath = utility::conversions::to_string_t(utility::conversions::to_utf8string(user) + "\\results" + mon + "_" + yr + ".pgn");
			if (month < 10)
			{
				mon = "0" + mon;
			}
			auto fileBuffer = std::make_shared<streambuf<uint8_t>>();
			file_buffer<uint8_t>::open(filePath, std::ios::out)
				.then([=](streambuf<uint8_t> outFile) -> pplx::task<http_response> {
				*fileBuffer = outFile;
				// Create http_client to send the request.
				http_client client(U("https://api.chess.com/pub/player"));


				// Build request URI and start the request.
				utility::string_t uri = utility::conversions::to_string_t("/games/" + yr + "/" + mon + "/pgn");
				uri = user + uri;
				uri_builder builder((uri));
				return client.request(methods::GET, builder.to_string());
					})
				// Write the response body into the file buffer.
						.then([=](http_response response) -> pplx::task<size_t> {
						printf("Response code %u .\n", response.status_code());

						return response.body().read_to_end(*fileBuffer);
							})

						// Close the file buffer.
								.then([=](size_t) { return fileBuffer->close(); })

								// Wait for the entire response body to be written into the file.
								.wait();
		}
	}
}
	catch (bad_lexical_cast &)
	{
		std::cout << "Caught an error BAD CAST!" << std::endl;
	}
	return 0;
}