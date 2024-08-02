const char css_file[] PROGMEM = R"=====(
label {
        font-weight: bold;
        font-size:2vw;
        color:green;
}
select {
        font-size:2vw;
        padding: 1em;
        color: white; /* Text color */
        background-color: green; /* Background color */
        border-radius: .5em; /* Rounded corners */
}
.buttons {
        font-size:3vw;
        background:teal;
        border-radius:.3em;
        color:white
}
::-webkit-file-upload-button {
        color:white;
        background:green;
}
img {
        display: block;
        max-width: 100%;
}
body
{
        color: white;
        background-color: black;
        margin-left: 0;
        margin-top: 0;
        margin-right: 0;
        text-align: center;
}
main
{
        max-width: 800px ;
        margin: auto ;
        display: block;
}
h1
{
        color: orange;
        text-align: center;
}
h2
{
        text-align: center;
        color: teal;
        margin: 5%;
}
.loader
{
  position: fixed;
  top: 0;
  left: 0;
  width: 100vw;
  height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  background: #333333;
  transition: opacity 0.75, visibility 0.75;
}
.loader--hidden
{
  opacity: 0;
  visibility: hidden;
}
.loader::after
{
  content: "";
  width: 75px;
  height: 75px;
  border: 15px solid #dddddd;
  border-top-color: #009578;
  border-radius: 50%;
  animation: loading 0.75s ease infinite;
}
@keyframes loading
{
  from { transform: rotate(0turn)}
  to { transform: rotate(1turn)}
}
)=====";
