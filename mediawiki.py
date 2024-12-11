from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.remote.webelement import WebElement
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.keys import Keys

from pathlib import Path


import os
import requests
import pyautogui
import time
import random
import sqlite3


def handle_original_file(url: str, taken_with: str, cb_file_url_manufacture_model):
    driver = webdriver.Chrome()
    driver.get(url)
    driver.implicitly_wait(5)

    class_fullMedia = 'fullMedia'
    class_mw_imagepage_section_metadata = 'mw-imagepage-section-metadata'
    class_mw_metadata = 'mw_metadata'
    class_exif_make = 'exif-make'
    class_exif_model = 'exif-model'

    # tags of item
    tag_name_bdi = 'bdi'

    # common tags
    tag_name_td = 'td'
    tag_name_a = 'a'

    attr_href = 'href'

    # get original file url
    original_img_url = driver.find_element(by=By.CLASS_NAME, value=class_fullMedia).find_element(by=By.TAG_NAME, value=tag_name_bdi).find_element(by=By.TAG_NAME, value=tag_name_a).get_attribute(attr_href)

    # if image
    # get Camera manufacturer and

    manufacturer = taken_with
    model = taken_with

    print(f'------------taken_with: {taken_with}')

    try:
        exif = driver.find_element(by=By.CLASS_NAME, value=class_mw_imagepage_section_metadata).find_element(by=By.CLASS_NAME, value=class_mw_metadata)
        manufacturer = exif.find_element(by=By.CLASS_NAME, value=class_exif_make).find_element(by=By.TAG_NAME, value=tag_name_a).text
        model = exif.find_element(by=By.CLASS_NAME, value=class_exif_model).find_element(by=By.TAG_NAME, value=tag_name_td).find_element(by=By.TAG_NAME, value=tag_name_a).text
    except:
        pass

    # if video only download
    if not cb_file_url_manufacture_model:
        print(f'uri: {original_img_url}, manufacture: {manufacturer}, mode: {model}')
    else:
        print('execute callback: cb_file_url_manufacture_model')
    cb_file_url_manufacture_model(manufacturer, model, original_img_url)

    driver.quit()


def handle_image_video_link(brand: str, url: str, cb_when_found_url):
    driver = webdriver.Chrome()
    driver.get(url)
    driver.implicitly_wait(5)

    id_mv_category_media = 'mw-category-media'
    id_GallerySlideStartButtons = 'GallerySlideStartButtons'
    class_gallery = 'gallery'
    class_gallerybox = 'gallerybox'
    class_thumb = 'thumb'
    class_gallerytext = 'gallerytext'

    tag_name_a = 'a'
    tag_name_h2 = 'h2'

    attr_href = 'href'

    category_media = driver.find_element(by=By.ID, value=id_mv_category_media)
    gallery = category_media.find_element(by=By.CLASS_NAME, value=class_gallery)

    title = category_media.find_element(by=By.TAG_NAME, value=tag_name_h2).text
    # start with Taken with or taken with, so ignore T
    str_aken_with = 'aken with '
    taken_with = ''
    if title.find(str_aken_with) != -1:
        taken_with = title[title.find(str_aken_with)+len(str_aken_with):-2]
    else:
        taken_with = title.replace('\"', '')

    img_meta_datas = gallery.find_elements(by=By.CLASS_NAME, value=class_gallerybox)
    for meta in img_meta_datas:
        text = meta.find_element(by=By.CLASS_NAME, value=class_gallerytext)
        href = text.find_element(by=By.TAG_NAME, value=tag_name_a).get_attribute(attr_href)
        if not cb_when_found_url:
            print(f'found:{brand}: {href}')
        else:
            print(f'---- [{brand}]: {href}')
            cb_when_found_url(brand, href, taken_with)
            # time.sleep(random.randint(1, 5))
    driver.quit()


def haneld_root_url(brand: str, url: str, cb_when_in_tail_node):
    driver = webdriver.Chrome()
    driver.get(url)
    driver.implicitly_wait(5)

    state_init = 'mw-category-group'
    state_tree_section = 'CategoryTreeSection'
    state_children = 'CategoryTreeChildren'
    state_item = 'CategoryTreeItem'
    # spec
    spec_taken_with = 'Taken with'
    spec_gopro_cameras = 'GoPro cameras'

    # class of item
    class_bullet = 'CategoryTreeBullet'
    class_empty_bullet = 'CategoryTreeEmptyBullet'
    class_CategoryTreeToggle = 'CategoryTreeToggle'
    class_CategoryTreeToggleHandlerAttached = 'CategoryTreeToggleHandlerAttached'

    # attr of item
    attr_aria_expand = 'aria-expanded'
    attr_data_ct_title = 'data-ct-title'
    attr_href = 'href'

    # tags of item
    tag_name_bdi = 'bdi'

    # common tags
    tag_name_a = 'a'

    wait_short_time = 1
    wait_medium_time = 3

    categorys = driver.find_elements(by=By.CLASS_NAME, value=state_init)
    que = [(state_init, category) for category in categorys]
    while len(que) > 0:
        (state, node) = que.pop(0)
        if state == state_init:
            sections = node.find_elements(by=By.CLASS_NAME, value=state_tree_section)
            for section in sections:
                if section.text.find(spec_taken_with) != -1 or section.text.find(spec_gopro_cameras) != -1:
                    que += [(state_tree_section, section)]
        elif state == state_tree_section:
            item = node.find_element(by=By.CLASS_NAME, value=state_item)
            try:
                bullet = item.find_element(by=By.CLASS_NAME, value=class_bullet)
                print(f'execute click')
                bullet.click()
                driver.implicitly_wait(wait_medium_time)
                children = node.find_element(by=By.CLASS_NAME, value=state_children)
                que += [(state_children, children)]
            except:
                # handle tail node
                bdi = item.find_element(by=By.TAG_NAME, value=tag_name_bdi)
                href = bdi.find_element(by=By.TAG_NAME, value=tag_name_a).get_attribute(attr_href)
                model = bdi.text.removeprefix('Taken with ')
                if not cb_when_in_tail_node:
                    print(f'model: {model}, uri: {href}, brand: {brand}')
                else:
                    cb_when_in_tail_node(model, href, brand)

        elif state == state_children:
            sections = node.find_elements(by=By.CLASS_NAME, value=state_tree_section)
            driver.implicitly_wait(wait_medium_time)
            que += [(state_tree_section, section) for section in sections]
    driver.quit()
    print(f'{brand} bfs end...')

def cb_when_in_tail_node(model: str, href: str, brand: str):
    print(f'model: {model}, href: {href}')
    base_dir = Path('D:/wiki_media')
    if not os.path.exists(str(base_dir)):
        os.mkdir(str(base_dir))

    def handle_img_desc(desc: str, taken_with: str):
        def download(manufacture: str, model: str, url: str):
            m_dir = base_dir/manufacture
            model_dir = m_dir/model
            if not os.path.exists(str(m_dir)):
                os.mkdir(str(m_dir))
            if not os.path.exists(str(model_dir)):
                os.mkdir(str(model_dir))
            response = requests.get(url)
            file_path = model_dir/(url.split('/')[-1])
            driver = webdriver.Chrome()
            driver.get(url)
            driver.implicitly_wait(8)

            print(f'{str(file_path)} download...................')
            pyautogui.hotkey('ctrl', 's')
            time.sleep(1.0)
            # pyautogui.typewrite(str(file_path), interval=0.2)
            pyautogui.typewrite(str(file_path))
            pyautogui.press('enter')
            time.sleep(10.0)
            driver.quit()
        handle_original_file(desc, taken_with, download)

    def record(brand: str, desc: str, taken_with: str):
        base = Path('D:/Users/Desktop/selenium')
        db_name = brand + '.db'
        db_path = base/db_name
        try:
            conn = sqlite3.connect(str(db_path))
            cursor = conn.cursor()
            create_table_comm = 'CREATE TABLE IF NOT EXISTS camera_file_urls (id INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE, taken_with TEXT NOT NULL, url TEXT NOT NULL)'
            cursor.execute(create_table_comm)
            conn.commit()

            sql = 'INSERT INTO camera_file_urls (taken_with, url) VALUES (?, ?)'
            cursor.execute(sql, (taken_with, desc))
            conn.commit()
            conn.close()
        except:
            pass

    # handle_image_video_link(href, handle_img_desc)
    handle_image_video_link(brand, href, record)

# haneld_root_url(url_, cb_when_in_tail_node)


# https://commons.wikimedia.org/wiki/Category:Cameras_by_brand
urls_ = {
    'dji': 'https://commons.wikimedia.org/wiki/Category:DJI_(company)',
    'gopro': 'https://commons.wikimedia.org/wiki/Category:GoPro',
    'insta360': 'https://commons.wikimedia.org/wiki/Category:Insta360_cameras',
    'sony': 'https://commons.wikimedia.org/wiki/Category:Sony_cameras',
    'canon': 'https://commons.wikimedia.org/wiki/Category:Canon_cameras'
}

for brand, url in urls_.items():
    haneld_root_url(brand, url, cb_when_in_tail_node)
