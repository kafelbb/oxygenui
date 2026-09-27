
const linksContainer = document.querySelector('.btns');
const urlMap = {
  'github': 'https://github.com/kafelbb/oxygenui',
  'wiki': 'https://github.com/kafelbb/oxygenui/blob/main/readme/wiki.md',
  'demo.mp4': 'page/images/o.mp4'
};

linksContainer.addEventListener('click', (event) => {
  const button = event.target.closest('.btn');
  if (!button) return;

  event.preventDefault();

  const buttonText = button.textContent.trim().toLowerCase();
  const targetUrl = urlMap[buttonText];

  if (targetUrl) {
    window.open(targetUrl, '_blank');
  }
});
