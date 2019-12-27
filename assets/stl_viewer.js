// Based on code by Anthony Biondo, licensed under the MIT license, from
// https://tonybox.net/posts/simple-stl-viewer/
function STLViewer(model, elementID, color) {
  color = parseInt(color, 16);
  var elem = document.getElementById(elementID)
  var camera = new THREE.PerspectiveCamera(70, elem.clientWidth/elem.clientHeight, 1, 1000);
  var renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
  renderer.setSize(elem.clientWidth, elem.clientHeight);
  elem.appendChild(renderer.domElement);
  window.addEventListener('resize', function () {
      renderer.setSize(elem.clientWidth, elem.clientHeight);
      camera.aspect = elem.clientWidth/elem.clientHeight;
      camera.updateProjectionMatrix();
  }, false);
  var controls = new THREE.OrbitControls(camera, renderer.domElement);
  controls.enableDamping = true;
  controls.rotateSpeed = 0.1;
  controls.dampingFactor = 0.1;
  controls.enableZoom = true;
  controls.autoRotate = false;
  controls.autoRotateSpeed = .75;
  var scene = new THREE.Scene();
  scene.add(new THREE.HemisphereLight(0xffffff, 1.5));

  (new THREE.STLLoader()).load(model, function (geometry) {
      var material = new THREE.MeshPhongMaterial({
          color: color,
          specular: 100,
          shininess: 100 });
      var mesh = new THREE.Mesh(geometry, material);
      scene.add(mesh);

      var middle = new THREE.Vector3();
      geometry.computeBoundingBox();
      geometry.boundingBox.getCenter(middle);
      mesh.position.x = -1 * middle.x;
      mesh.position.y = -1 * middle.y;
      mesh.position.z = -1 * middle.z;

      var largestDimension = Math.max(geometry.boundingBox.max.x,
                          geometry.boundingBox.max.y,
                          geometry.boundingBox.max.z)

      const fov = camera.fov * ( Math.PI / 180 );
      camera.position.z = Math.abs( largestDimension * 1.2 / 4 * Math.tan( fov * 2 ) );
      // camera.position.z = largestDimension;

      var animate = function () {
          requestAnimationFrame(animate);
          controls.update();
          renderer.render(scene, camera);
      };

      animate();
  });
}