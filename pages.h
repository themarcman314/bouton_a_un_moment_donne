const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">

	<title>Configuration page</title>
	<link rel="stylesheet" href="/style.css" />
</head>

<body>
<div class="loader" id="loader"></div>
<h1>Upload</h1>

<form id="upload" action="/edit" form method="post" enctype="multipart/form-data">
        <input 
            id="file-upload"
            type="file"
            name="uploadfile"
            accept=".mp3"
            class="buttons">
        <input 
            type="submit"
            value="Upload file" 
            class="buttons">
</form>

<img id="photo_du_foyer" src="/foyer.jpg" alt="Une foto du foyer">

<h1>Play</h1>
<form action="/select" method="post">
    <label for="fileSelect">Choose a file to play:</label>
        <select name="file" id="fileSelect">
</form>

<script>
    window.addEventListener("load", () => {
      document.querySelector(".loader").classList.add("loader--hidden");
    });
    document.getElementById('upload').addEventListener('submit', function() {
        document.getElementById('loader').classList.remove('loader--hidden');
    });
</script>
)=====";


const char toggleLED_html[] PROGMEM = R"=====(
<h1>Hello this is the toogle led page</h1>
<a href="/toggle">
    <button class="buttons">Toggle LED</button>
</a>
</body>
</html>
)=====";

const char redirect_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
  <script type="text/javascript">
    if (/Android/i.test(navigator.userAgent)) {
        window.location.href = "intent://192.168.1.1#Intent;scheme=http;package=com.android.chrome;end";
    } else {
        window.location.href = "http://192.168.1.1";
    }
</script>

</body>
</html>
)=====";
