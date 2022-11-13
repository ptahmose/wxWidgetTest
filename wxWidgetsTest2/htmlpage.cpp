#include "htmlpage.h"

const char* html_page = R"_(


<!DOCTYPE html>
<html>
<head>
    <title>wxWidgetsTest2</title>
    <script>
        function get_options() {
            var recursive = document.getElementById('recursivecheckbox').checked;
            var whattocompress = document.getElementById('whattocompresscombobox').value;
            var compressionlevel = parseInt(document.getElementById('compressionlevelnumerictextbox').value, 10);
            var source_folder = document.getElementById('sourcefolderinputtextbox').value;
            var destination_folder = document.getElementById('destinationfolderinputtextbox').value;
            return {
                'recursive': recursive,
                'whattocompress': whattocompress,
                'compressionlevel': compressionlevel,
                'sourcefolder': source_folder,
                'destinationfolder': destination_folder
            };
            //alert(recursive +  " | " + whattocompress + "| " + compressionlevel);
        }
        function add_to_log(characters_to_delete, text) {
            if (characters_to_delete == 0) {
                document.getElementById('logtextbox').value += text;
            }
            else {
                var t = document.getElementById('logtextbox').value;
                document.getElementById('logtextbox').value = t.slice(0, -characters_to_delete) + text;
            }
        }
        function set_statistics(statistics_object) {
            if (statistics_object.hasOwnProperty('files_processed')) {
                document.getElementById('statistics_files_processed').innerHTML = statistics_object.files_processed.toLocaleString('en');
            }
            if (statistics_object.hasOwnProperty('data_size_of_files_processed_before_compression')) {
                document.getElementById('statistics_data_size_of_files_processed_before_compression').innerHTML = statistics_object.data_size_of_files_processed_before_compression.toLocaleString('en');
            }
            if (statistics_object.hasOwnProperty('data_size_of_files_processed_after_compression')) {
                document.getElementById('statistics_data_size_of_files_processed_after_compression').innerHTML = statistics_object.data_size_of_files_processed_after_compression.toLocaleString('en');
            }
        }
        function set_operational_state(is_running) {
            if (is_running) {
                document.getElementById('startbutton').setAttribute('disabled', '');
                document.getElementById('stopbutton').removeAttribute('disabled');
            }
            else {
                document.getElementById('startbutton').removeAttribute('disabled');
                document.getElementById('stopbutton').setAttribute('disabled', '');
            }
        }
    </script>
</head>
<body>
    <h1>wxWidgetsTest2</h1>

    <p>
        <label for="name">Source folder:</label>
        <input type="text" id="sourcefolderinputtextbox">
        <button type="button" onclick="(function(){t=document.getElementById('sourcefolderinputtextbox').value;window.wx_msg.postMessage({'id':'sourcefolderinputtextbox','arg':t});})();">...</button>
    </p>
    <p>
        <label for="name">Destination folder:</label>
        <input type="text" id="destinationfolderinputtextbox">
        <button type="button" onclick="(function(){t=document.getElementById('destinationfolderinputtextbox').value;window.wx_msg.postMessage({'id':'destinationfolderinputtextbox','arg':t});})();">...</button>
    </p>

    <p>
        <label for="recursivecheckbox"> recursive</label>
        <input type="checkbox" id="recursivecheckbox" value="true">
    </p>
    <p>
        <label for="whattocompresscombobox">What to compress:</label>
        <select id="whattocompresscombobox">
            <option value="only_uncompressed">only uncompressed subblocks</option>
            <option value="uncompressed_and_zstd">uncompressed & zstd compressed subblocks</option>
            <option value="uncompressed_and_zstd_and_jpgxr">uncompressed & zstd & JPGXR compressed subblocks</option>
        </select>
    </p>
    <p>
        <label for="compressionlevelnumerictextbox">compression level:</label>
        <input type="number" id="compressionlevelnumerictextbox" max="22" min="-131000" value="0">
    </p>

    <p>
        <button type="button" id="startbutton" onclick="window.wx_msg.postMessage({'id':'startbutton','arg':get_options()});">Start</button>
        <button type="button" id="stopbutton" disabled onclick="(function(){t=document.getElementById('sourcefolderinputtextbox').value;window.wx_msg.postMessage({'id':'stopbutton','arg':t});})();">Stop</button>
    </p>

    <p>
        <table style="width:100%">
            <tr>
                <th>reading</th>
                <th>value</th>
            </tr>
            <tr>
                <td>files processed:</td>
                <td><div id="statistics_files_processed">0</div></td>
            </tr>
            <tr>
                <td>original total file size:</td>
                <td><div id="statistics_data_size_of_files_processed_before_compression">0</div></td>
            </tr>
            <tr>
                <td>compressed total file size:</td>
                <td><div id="statistics_data_size_of_files_processed_after_compression">0</div></td>
            </tr>
        </table>
    </p>

    <p>
        <textarea id="logtextbox" style="width:100%;height:100%" readonly></textarea>
    </p>

</body>
</html>






)_";