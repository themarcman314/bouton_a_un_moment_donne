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

<form id="upload" action="/edit" method="post" enctype="multipart/form-data">
  <input 
      id="file-upload"
      type="file"
      name="uploadfile"
      accept="audio/mpeg"
      class="buttons">
  <input 
      type="submit"
      value="Upload file"
      class="buttons">
  
  <div id="error" style="color: red;"></div>

</form>
<p>(Filename must only have alphanumeric characters, '-', '_', '.' and '/'. It must also be at most 29 characters long.)</p>
<p>(Le nom d'un fichier doit uniquement être composé de caractères alphanumériques, '-', '_', '.' et '/'. Il doit aussi faire 29 caractères au maximum.)</p>

<img id="photo_du_foyer" src="/foyer.jpg" alt="Une foto du foyer">

<h1>Play</h1>
<form id="play" action="/select" method="post">
    <label for="fileSelect">Choose the file to be played:</label>
        <select name="file" id="fileSelect">


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

const char scripts[] PROGMEM = R"=====(
<script>
  window.addEventListener("load", () => {
    document.querySelector(".loader").classList.add("loader--hidden");
  });
  // document.getElementById('upload').addEventListener('submit', function() {
  //   document.getElementById('loader').classList.remove('loader--hidden');
  // });

  document.getElementById('upload').addEventListener('submit', function(event) {
    const fileInput = document.getElementById('file-upload');
    const errorDiv = document.getElementById('error');
    errorDiv.textContent = '';

  if (fileInput.files.length === 0) {
    errorDiv.textContent = 'No file selected';
    event.preventDefault();
    return;
  }
  const file = fileInput.files[0];
  const filename = file.name;
  const maxFilenameLength = 29; // Maximum length without the null terminator

  // Check filename length
  if (filename.length > maxFilenameLength) {
    errorDiv.textContent = 'Filename is too long.';
    event.preventDefault();
    return;
  }

  // Regular expression for allowed characters: alphanumeric, -, _, ., /
  const allowedCharacters = /^[a-zA-Z0-9\-_.\/ ]+$/;

  // Check if filename contains only allowed characters
  if (!allowedCharacters.test(filename)) {
    errorDiv.textContent = 'Filename contains invalid characters.';
    event.preventDefault();
    return;
  }
  document.getElementById('loader').classList.remove('loader--hidden');
});


  document.getElementById('del').addEventListener('submit', function() {
    document.getElementById('loader').classList.remove('loader--hidden');
  });
  document.getElementById('play').addEventListener('submit', function() {
    document.getElementById('loader').classList.remove('loader--hidden');
  });
</script>
)=====";
