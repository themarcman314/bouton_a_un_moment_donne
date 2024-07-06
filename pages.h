const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<head>
<title>File Upload</title>
	<link rel="stylesheet" href="/style.css" />
</head>

<h1>Select your Upload file</h1>
<form action="/edit" form method="post" enctype="multipart/form-data">
<label for="file">File</label>
<input id="file" name="file" type="file"/>
<button>Upload</button>
</form>
<button onclick="location.href='/list'">Get List</button>
<img id="photo_du_foyer" src="/foyer.jpg" alt="Une foto du foyer">
)=====";


const char toggleLED_html[] PROGMEM = R"=====(
<h1>Hello this is the toogle led page</h1>\
<a href=\"/toggle\"><button>Toggle LED</button></a>\
</button>
)=====";
