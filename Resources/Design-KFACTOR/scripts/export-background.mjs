import puppeteer from 'puppeteer';

const args = process.argv.slice(2);
const argValue = (flag, fallback) => {
  const index = args.indexOf(flag);
  return index !== -1 && args[index + 1] ? args[index + 1] : fallback;
};

const url = argValue('--url', 'http://127.0.0.1:4174/');
const outPath = argValue('--out', './kfactor-background.png');

const browser = await puppeteer.launch({ headless: 'new' });
const page = await browser.newPage();

await page.setViewport({ width: 1800, height: 1200, deviceScaleFactor: 1 });
await page.goto(url, { waitUntil: 'networkidle0' });
await page.waitForSelector('#kfactor-plugin');

const element = await page.$('#kfactor-plugin');
if (!element) {
  console.error('Could not find #kfactor-plugin element');
  await browser.close();
  process.exit(1);
}

await element.screenshot({ path: outPath });
const size = await element.boundingBox();
if (size) {
  console.log(`Exported ${Math.round(size.width)}x${Math.round(size.height)} to ${outPath}`);
}

await browser.close();
