echo "Switching to branch Main"
git checkout main

echo "Pulling latest changes from main"
git pull origin main

echo "Installing dependencies..."
npm install

echo "Building app..."
npm run build

echo "Deploying files to server..."
scp -r dist/* webtool@192.168.51.120:/var/www/192.168.51.120/

echo "Done!"