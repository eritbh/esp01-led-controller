// Web server setup and routing.

#ifndef ROUTING_HPP
#define ROUTING_HPP

#include <ESP8266WebServerSecure.h>

#include "CredentialManager.hpp"
#include "Neopixel.hpp"
#include "Secrets.hpp"

ESP8266WebServerSecure server(443);
CredentialManager credentialManager("/credentials.txt", "global", DEFAULT_LOGIN, DEFAULT_PASSWORD);

// Utilities

//This function checks whether the current session has been authenticated. If not, a request for credentials is sent.
bool session_authenticated()
{
  return true; // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
  Serial.println("Checking authentication.");
  if (server.authenticateDigest(credentialManager.login, credentialManager.credentialHash))
  {
    Serial.println("Authentication confirmed.");
    return true;
  }
  else
  {
    Serial.println("Not authenticated. Requesting credentials.");
    server.requestAuthentication(DIGEST_AUTH, credentialManager.salt.c_str(), "Authentication failed");
    return false;
  }
}

// Route handlers

// GET /
void showHomepage()
{
  server.send(200, "text/html", R"HTML(
    <html>
      hi there it worked
    </html>
  )HTML");
}

// GET /changecreds
void showcredentialpage()
{
  Serial.println("Show credential page called.");
  if (!session_authenticated())
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }
  server.send(200, "text/html", R"HTML(
    <html>
      <h1>Login Credentials</h1>
      <form action="changecreds" method="post">
        <p>
          <label for="login">Login</label>
          <input type="text" name="login">
        </p>
        <p>
          <label for="password">New password</label>
          <input type="password" name="password">
        </p>
        <p>
          <label for="password_duplicate">New password (again):</label>
          <input type="password" name="password_duplicate">
        </p>
        <p>
          <button type="submit" name="newcredentials">Change</button>
        </p>
      </form>
    </html>
  )HTML");
}

// POST /changecreds
void handlecredentialchange()
{
  if (!session_authenticated())
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  String login = server.arg("login");
  String pw1 = server.arg("password");
  String pw2 = server.arg("password_duplicate");

  if (login != "" && pw1 != "" && pw1 == pw2)
  {

    credentialManager.edit(login, pw1);
    server.send(200, "text/plain", "Credentials updated");
  }
  else
  {
    server.send(200, "text/plain", "Malformed credentials");
  }
}

// GET /color
void GET_color()
{
  if (!session_authenticated())
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  server.send(200, "text/html", R"HTML(
    <html>
      <h1>Change Color</h1>
      <form action="color" method="post">
        <p>
          <label for="r">R</label>
          <input type="range" name="r" min="0" max="255">
        </p>
        <p>
          <label for="g">G</label>
          <input type="range" name="g" min="0" max="255">
        </p>
        <p>
          <label for="b">B</label>
          <input type="range" name="b" min="0" max="255">
        </p>
        <p>
          <button type="submit">Change color</button>
        </p>
      </form>
    </html>
  )HTML");
}

// POST /color
void setPattern()
{
  if (!session_authenticated())
  {
    server.send(401, "text/plain", "Unauthorized");
    return;
  }

  int patternLength = server.arg("length").toInt();
  uint32_t pattern[256];
  String patternString = server.arg("pattern");
  for (int i = 0; i < patternLength && i < 256; i += 1)
  {
    String color;
    for (int j = 0; j < patternString.length(); j += 1) {
      if (patternString[j] == ' ') {
        color = patternString.substring(0, j);
        patternString.remove(0, j + 1);
      }
    }
    if (color.length() < 1) {
      color = patternString.substring(0);
      patternString.remove(0);
    }
    pattern[i] = color.toInt();
  }
  updatePattern(patternLength - 1, pattern);
  server.send(204, "text/plain", String(patternLength - 1) + " " + patternString);
}

// Functions for getting the server to actually do things

void beginRouting()
{
  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  server.on("/", HTTP_GET, showHomepage);
  server.on("/changecreds", HTTP_GET, showcredentialpage);      //for this simple example, just show a simple page for changing credentials at the root
  server.on("/changecreds", HTTP_POST, handlecredentialchange); //handles submission of credentials from the client
  server.on("/color", HTTP_GET, GET_color);
  server.on("/pattern", HTTP_POST, setPattern);
  server.begin();
}

void handleRouting()
{
  server.handleClient();
}

#endif
