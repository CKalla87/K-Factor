import puppeteer from 'puppeteer';

const args = process.argv.slice(2);
const argValue = (flag, fallback) => {
  const index = args.indexOf(flag);
  return index !== -1 && args[index + 1] ? args[index + 1] : fallback;
};

const url = argValue('--url', 'http://127.0.0.1:4174/');

const browser = await puppeteer.launch({ headless: 'new' });
const page = await browser.newPage();
await page.setViewport({ width: 1800, height: 1200, deviceScaleFactor: 1 });
await page.goto(url, { waitUntil: 'networkidle0' });

await page.waitForSelector('#kfactor-plugin');

const layout = await page.evaluate(() => {
  const plugin = document.querySelector('#kfactor-plugin');
  if (!plugin) return null;

  const pluginRect = plugin.getBoundingClientRect();
  const elements = Array.from(document.querySelectorAll('[data-ui]'));

  const rects = {};
  for (const el of elements) {
    const name = el.getAttribute('data-ui');
    if (!name) continue;
    const rect = el.getBoundingClientRect();
    rects[name] = {
      x: Math.round(rect.left - pluginRect.left),
      y: Math.round(rect.top - pluginRect.top),
      width: Math.round(rect.width),
      height: Math.round(rect.height)
    };
  }

  return {
    width: Math.round(pluginRect.width),
    height: Math.round(pluginRect.height),
    rects
  };
});

if (!layout) {
  console.error('Could not find plugin container');
  await browser.close();
  process.exit(1);
}

console.log(JSON.stringify(layout, null, 2));
await browser.close();
